/*
* [The "BSD license"]
*  Copyright (c) 2012-2016 Marcel Schaible
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  3. The name of the author may not be used to endorse or promote products
*     derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
*  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
*  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package org.smallpearl.compiler;

import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.smallpearl.compiler.SymbolTable.*;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Collections;


public class SymbolTableVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;
    private boolean m_debug;

    public SymbolTable symbolTable;
    private SymbolTableEntry m_currentEntry;
    private SymbolTable m_currentSymbolTable;
    private LinkedList<LinkedList<SemaphoreEntry>> m_listOfTemporarySemaphoreArrays;
    private LinkedList<LinkedList<BoltEntry>> m_listOfTemporaryBoltArrays;
    private LinkedList<ArrayDescriptor> m_listOfArrayDescriptors;

    private TypeDefinition m_type;
    private ParseTreeProperty<SymbolTable> m_symboltablePerContext = null;
    private ConstantPool m_constantPool = null;

    public SymbolTableVisitor(int verbose, ConstantPool constantPool) {

        m_debug = false;
        m_verbose = verbose;


        m_debug = true;
        if (m_verbose > 0) {
            System.out.println("Building new symbol table");
        }

        this.symbolTable = new org.smallpearl.compiler.SymbolTable.SymbolTable();
        this.m_listOfTemporarySemaphoreArrays = new LinkedList<LinkedList<SemaphoreEntry>>();
        this.m_listOfTemporaryBoltArrays = new LinkedList<LinkedList<BoltEntry>>();
        this.m_listOfArrayDescriptors = new LinkedList<ArrayDescriptor>();
        this.m_symboltablePerContext = new ParseTreeProperty<SymbolTable>();
        this.m_constantPool = constantPool;
    }

    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitModule");
        }

        org.smallpearl.compiler.SymbolTable.ModuleEntry moduleEntry = new org.smallpearl.compiler.SymbolTable.ModuleEntry(ctx.ID().getText(), ctx, null);
        this.m_currentSymbolTable = this.symbolTable.newLevel(moduleEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable);

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.System_partContext) {
                    visitSystem_part((SmallPearlParser.System_partContext) c);
                } else if (c instanceof SmallPearlParser.Problem_partContext) {
                    visitProblem_part((SmallPearlParser.Problem_partContext) c);
                }
            }
        }


        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }


    @Override
    public Void visitProblem_part(SmallPearlParser.Problem_partContext ctx) {

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ScalarVariableDeclarationContext) {
                    visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext) c);
                } else if (c instanceof SmallPearlParser.SemaDeclarationContext) {
                    visitSemaDeclaration((SmallPearlParser.SemaDeclarationContext) c);
                } else if (c instanceof SmallPearlParser.BoltDeclarationContext) {
                    visitBoltDeclaration((SmallPearlParser.BoltDeclarationContext) c);
                } else if (c instanceof SmallPearlParser.TaskDeclarationContext) {
                    visitTaskDeclaration((SmallPearlParser.TaskDeclarationContext) c);
                } else if (c instanceof SmallPearlParser.DationSpecificationContext) {
                    visitDationSpecification((SmallPearlParser.DationSpecificationContext) c);
                } else if (c instanceof SmallPearlParser.DationDeclarationContext) {
                    visitDationDeclaration((SmallPearlParser.DationDeclarationContext) c);
                } else if (c instanceof SmallPearlParser.ProcedureDeclarationContext) {
                    visitProcedureDeclaration((SmallPearlParser.ProcedureDeclarationContext) c);
                } else if (c instanceof SmallPearlParser.LengthDefinitionContext) {
                    visitLengthDefinition((SmallPearlParser.LengthDefinitionContext) c);
                } else if (c instanceof SmallPearlParser.StructVariableDeclarationContext) {
                    visitStructVariableDeclaration((SmallPearlParser.StructVariableDeclarationContext) c);
                }
            }
        }

        return null;
    }

    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        Boolean isMain = false;
        Boolean isGlobal = false;
        SmallPearlParser.PriorityContext priority = null;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: vistTaskDeclaration");
        }

        SymbolTableEntry entry = this.m_currentSymbolTable.lookup(ctx.ID().toString());
        if (entry != null) {
            throw new DoubleDeclarationException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        isMain = ctx.task_main() != null;
        if (ctx.priority() != null) {
            priority = ctx.priority();
        }

        TaskEntry taskEntry = new TaskEntry(ctx.ID().getText(), priority, isMain, isGlobal, ctx, this.m_currentSymbolTable);
        this.m_currentSymbolTable = this.m_currentSymbolTable.newLevel(taskEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable);

        visitChildren(ctx);

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        String globalId = null;
        LinkedList<FormalParameter> formalParameters = null;
        ExpressionResult resultType = null;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitProcedureDeclaration");
        }

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.ResultAttributeContext) {
                resultType = new ExpressionResult(getResultAttribute((SmallPearlParser.ResultAttributeContext) c));
            } else if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                SmallPearlParser.GlobalAttributeContext globalCtx = (SmallPearlParser.GlobalAttributeContext) c;
                globalId = ctx.ID().getText();
            } else if (c instanceof SmallPearlParser.ListOfFormalParametersContext) {
                SmallPearlParser.ListOfFormalParametersContext listOfFormalParametersContext = (SmallPearlParser.ListOfFormalParametersContext) c;
                formalParameters = getListOfFormalParameters((SmallPearlParser.ListOfFormalParametersContext) c);
            }
        }

        SymbolTableEntry entry = this.m_currentSymbolTable.lookup(ctx.ID().toString());
        if (entry != null) {
            throw new DoubleDeclarationException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ProcedureEntry procedureEntry = new ProcedureEntry(ctx.ID().getText(), formalParameters, resultType, globalId, ctx, this.m_currentSymbolTable);
        this.m_currentSymbolTable = this.m_currentSymbolTable.newLevel(procedureEntry);

        /* Enter formal parameter into the local symboltable of this procedure */
        if (formalParameters != null && formalParameters.size() > 0) {
            for (FormalParameter formalParameter : formalParameters) {
                VariableEntry param = new VariableEntry(formalParameter.name, formalParameter.type, formalParameter.assignmentProtection, null, null);
                this.m_currentSymbolTable.enter(param);
            }
        }

        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable);

        visitChildren(ctx);

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    private TypeDefinition getResultAttribute(SmallPearlParser.ResultAttributeContext ctx) {
        visitChildren(ctx.resultType());
        return m_type;
    }

    private LinkedList<FormalParameter> getListOfFormalParameters(SmallPearlParser.ListOfFormalParametersContext ctx) {
        LinkedList<FormalParameter> listOfFormalParameters = new LinkedList<FormalParameter>();

        if (ctx != null) {
            for (int i = 0; i < ctx.formalParameter().size(); i++) {
                getFormalParameter(listOfFormalParameters, ctx.formalParameter(i));
            }
        }

        return listOfFormalParameters;
    }

    private Void getFormalParameter(LinkedList<FormalParameter> listOfFormalParameters, SmallPearlParser.FormalParameterContext ctx) {
        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                FormalParameter formalParameter = null;
                String name = null;
                Boolean assignmentProtection = false;
                Boolean passIdentical = false;

                name = ctx.ID(i).getText();

                if (ctx.assignmentProtection() != null) {
                    assignmentProtection = true;
                }

                if (ctx.passIdentical() != null) {
                    passIdentical = true;
                }

                getParameterType(ctx.parameterType());
                listOfFormalParameters.add(new FormalParameter(name, m_type, assignmentProtection));
            }
        }

        return null;
    }

    private Void getParameterType(SmallPearlParser.ParameterTypeContext ctx) {
        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.SimpleTypeContext) {
                visitSimpleType(ctx.simpleType());
            }
        }

        return null;
    }

    @Override
    public Void visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        String blockLabel;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitBlock_statement");
        }

        if (ctx.ID() != null) {
            blockLabel = ctx.ID().toString();
        } else {
            blockLabel = "?anonymous?";
        }

        BlockEntry blockEntry = new BlockEntry(blockLabel, ctx, m_currentSymbolTable);

        m_currentSymbolTable = m_currentSymbolTable.newLevel(blockEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable);

        visitChildren(ctx);

        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitScalarVariableDeclaration(SmallPearlParser.ScalarVariableDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitScalarVariableDeclaration");
        }

        if (ctx != null) {
            for (int i = 0; i < ctx.variableDenotation().size(); i++) {
                visitVariableDenotation(ctx.variableDenotation().get(i));
            }
        }

        return null;
    }

    @Override
    public Void visitVariableDenotation(SmallPearlParser.VariableDenotationContext ctx) {
        boolean hasGlobalAttribute = false;
        boolean hasAllocationProtection = false;

        ArrayList<String> identifierDenotationList = null;
        ArrayList<Initializer> initElementList = null;

        m_type = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.IdentifierDenotationContext) {
                    identifierDenotationList = getIdentifierDenotation((SmallPearlParser.IdentifierDenotationContext) c);
                } else if (c instanceof SmallPearlParser.AllocationProtectionContext) {
                    hasAllocationProtection = true;
                } else if (c instanceof SmallPearlParser.TypeAttributeContext) {
                    visitTypeAttribute((SmallPearlParser.TypeAttributeContext) c);
                } else if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                    hasGlobalAttribute = true;
                } else if (c instanceof SmallPearlParser.InitialisationAttributeContext) {
                    initElementList = getInitialisationAttribute((SmallPearlParser.InitialisationAttributeContext) c);
                }
            }

            if (initElementList != null && identifierDenotationList.size() != initElementList.size()) {
                throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            int j = 0;
            for (int i = 0; i < identifierDenotationList.size(); i++) {
                VariableEntry variableEntry = null;

                if (initElementList != null && initElementList.size() > 0) {
                    if (j < initElementList.size()) {
                        Initializer initializer = initElementList.get(j);
                        fixPrecision(ctx,m_type,initializer);
                        variableEntry = new VariableEntry(identifierDenotationList.get(i), m_type, hasAllocationProtection, ctx, initElementList.get(j));
                        j++;
                    } else {
                        Initializer initializer = initElementList.get(j - 1);
                        fixPrecision(ctx,m_type,initializer);
                        variableEntry = new VariableEntry(identifierDenotationList.get(i), m_type, hasAllocationProtection, ctx, initElementList.get(j - 1));
                    }
                } else {
                    variableEntry = new VariableEntry(identifierDenotationList.get(i), m_type, hasAllocationProtection, ctx, null);
                }

                if (!m_currentSymbolTable.enter(variableEntry)) {
                    SymbolTableEntry entry = m_currentSymbolTable.lookupLocal(identifierDenotationList.get(i));
                    if (entry != null) {
                        if (entry instanceof VariableEntry) {
                            if (((VariableEntry) entry).getLoopControlVariable()) {
                                throw new SemanticError(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine(), "Loop control variable cannot be declared");
                            }
                        }
                    }

                    throw new SemanticError(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine(), "Double definition of " + identifierDenotationList.get(i));
                }
            }
        }

        return null;
    }


    private void fixPrecision(ParserRuleContext ctx, TypeDefinition type, Initializer initializer) {
        if( type instanceof TypeFixed)
        {
            TypeFixed typ = (TypeFixed)type;

            if ( initializer.getConstant() instanceof ConstantFixedValue) {
                ConstantFixedValue value = (ConstantFixedValue) initializer.getConstant();
                value.setPrecision(typ.getPrecision());
            }
            else {
                throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if( type instanceof TypeFloat)

        {
            TypeFloat typ = (TypeFloat) type;

            if ( initializer.getConstant() instanceof ConstantFloatValue) {
                ConstantFloatValue value = (ConstantFloatValue) initializer.getConstant();
                value.setPrecision(typ.getPrecision());
            }
            else if ( initializer.getConstant() instanceof ConstantFixedValue) {
                ConstantFixedValue fixedValue = (ConstantFixedValue) initializer.getConstant();
                ConstantFloatValue floatValue = new ConstantFloatValue((double)fixedValue.getValue(), typ.getPrecision());
                m_constantPool.add(floatValue);
                initializer.setConstant(floatValue);
            }
            else {
                throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
    }

    @Override
    public Void visitTypeAttribute(SmallPearlParser.TypeAttributeContext ctx) {
        if (ctx.simpleType() != null) {
            visitSimpleType(ctx.simpleType());
        }
        else if (ctx.typeReference() != null ){
            visitTypeReference(ctx.typeReference());
        }
        return null;
    }

    @Override
    public Void visitSimpleType(SmallPearlParser.SimpleTypeContext ctx) {
        if (ctx != null) {
            if (ctx.typeInteger() != null) {
                visitTypeInteger(ctx.typeInteger());
            } else if (ctx.typeDuration() != null) {
                visitTypeDuration(ctx.typeDuration());
            } else if (ctx.typeBitString() != null) {
                visitTypeBitString(ctx.typeBitString());
            } else if (ctx.typeFloatingPointNumber() != null) {
                visitTypeFloatingPointNumber(ctx.typeFloatingPointNumber());
            } else if (ctx.typeTime() != null) {
                visitTypeTime(ctx.typeTime());
            } else if (ctx.typeCharacterString() != null) {
                visitTypeCharacterString(ctx.typeCharacterString());
            }
        }

        return null;
    }

    @Override
    public Void visitTypeReference(SmallPearlParser.TypeReferenceContext ctx) {
        visitChildren(ctx);
        m_type = new TypeReference(m_type);
        return null;
    }

    @Override
    public Void visitTypeReferenceSimpleType(SmallPearlParser.TypeReferenceSimpleTypeContext ctx) {
        visitSimpleType(ctx.simpleType());
        return null;
    }

    @Override
    public Void visitTypeReferenceTaskType(SmallPearlParser.TypeReferenceTaskTypeContext ctx) {
        m_type = new TypeTask();
        return null;
    }

    @Override
    public Void visitTypeReferenceSemaType(SmallPearlParser.TypeReferenceSemaTypeContext ctx) {
        m_type = new TypeSemaphore();
        return null;
    }

    @Override
    public Void visitTypeReferenceBoltType(SmallPearlParser.TypeReferenceBoltTypeContext ctx) {
        m_type = new TypeBolt();
        return null;
    }

    @Override
    public Void visitTypeReferenceProcedureType(SmallPearlParser.TypeReferenceProcedureTypeContext ctx) {
        m_type = new TypeProcedure();
        return null;
    }

    @Override
    public Void visitTypeReferenceInterruptType(SmallPearlParser.TypeReferenceInterruptTypeContext ctx) {
        m_type = new TypeInterrupt();
        return null;
    }

    @Override
    public Void visitTypeReferenceSignalType(SmallPearlParser.TypeReferenceSignalTypeContext ctx) {
        m_type = new TypeSignal();
        return null;
    }

    @Override
    public Void visitTypeInteger(SmallPearlParser.TypeIntegerContext ctx) {
        Integer size;

        size = m_currentSymbolTable.lookupDefaultFixedLength();

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.MprecisionContext) {
                    size = Integer.parseInt(((SmallPearlParser.MprecisionContext) c).integerWithoutPrecision().IntegerConstant().getText());
                }
            }
        }

        m_type = new TypeFixed(size);
        return null;
    }

    @Override
    public Void visitTypeBitString(SmallPearlParser.TypeBitStringContext ctx) {
        int length = Defaults.BIT_LENGTH;

        if (ctx.IntegerConstant() != null) {
            length = Integer.parseInt(ctx.IntegerConstant().getText());
            if (length < 1 || length > 64) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        m_type = new TypeBit(length);

        return null;
    }

    @Override
    public Void visitTypeCharacterString(SmallPearlParser.TypeCharacterStringContext ctx) {
        int size = Defaults.CHARACTER_LENGTH;

        if (ctx.IntegerConstant() != null) {
            size = Integer.parseInt(ctx.IntegerConstant().getText());

            if (size < 1 || size > 255) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        m_type = new TypeChar(size);

        return null;
    }

    @Override
    public Void visitTypeFloatingPointNumber(SmallPearlParser.TypeFloatingPointNumberContext ctx) {
        int precision = Defaults.FLOAT_PRECISION;

        precision = m_currentSymbolTable.lookupDefaultFloatLength();

        if (ctx.IntegerConstant() != null) {
            precision = Integer.parseInt(ctx.IntegerConstant().getText());

            if (precision != Defaults.FLOAT_SHORT_PRECISION && precision != Defaults.FLOAT_LONG_PRECISION) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        m_type = new TypeFloat(precision);

        return null;
    }

    @Override
    public Void visitTypeDuration(SmallPearlParser.TypeDurationContext ctx) {
        m_type = new TypeDuration();
        return null;
    }

    private ArrayList<String> getIdentifierDenotation(SmallPearlParser.IdentifierDenotationContext ctx) {
        ArrayList<String> identifierDenotationList = new ArrayList<String>();

        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                identifierDenotationList.add(ctx.ID().get(i).toString());
            }
        }

        return identifierDenotationList;
    }

    @Override
    public Void visitTypeTime(SmallPearlParser.TypeTimeContext ctx) {

        if (ctx.type_clock() != null) {
            m_type = new TypeClock();
        } else if (ctx.type_duration() != null) {
            m_type = new TypeDuration();
        }

        return null;
    }

    @Override
    public Void visitType_clock(SmallPearlParser.Type_clockContext ctx) {
        m_type = new TypeClock();
        return null;
    }

    @Override
    public Void visitArrayVariableDeclaration(SmallPearlParser.ArrayVariableDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitArrayVariableDeclaration");
        }

        visitChildren(ctx);

        return null;
    }

    @Override
    public Void visitArrayDenotation(SmallPearlParser.ArrayDenotationContext ctx) {
        boolean hasGlobalAttribute = false;
        boolean hasAssigmentProtection = false;
        ArrayList<String> identifierDenotationList = new ArrayList<String>();;

        m_type = new TypeArray();

        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                identifierDenotationList.add(ctx.ID().get(i).toString());
            }
        }

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.DimensionAttributeContext) {
                    visitDimensionAttribute((SmallPearlParser.DimensionAttributeContext) c);
                } else if (c instanceof SmallPearlParser.AssignmentProtectionContext) {
                    hasAssigmentProtection = true;
                } else if (c instanceof SmallPearlParser.TypeAttributeForArrayContext) {
                    visitTypeAttributeForArray((SmallPearlParser.TypeAttributeForArrayContext)c);
                } else if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                    hasGlobalAttribute = true;
                }
            }

            addArrayDescriptor(new ArrayDescriptor(((TypeArray)m_type).getNoOfDimensions(),((TypeArray)m_type).getDimensions()));

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                VariableEntry variableEntry = new VariableEntry(identifierDenotationList.get(i), m_type,hasAssigmentProtection,null,null);
                if (!m_currentSymbolTable.enter(variableEntry)) {
                    System.out.println("ERR: Double definition of " + identifierDenotationList.get(i));
                }
            }
        }

        return null;
    }

    @Override
    public Void visitDimensionAttribute(SmallPearlParser.DimensionAttributeContext ctx) {
        visitChildren(ctx);
        return null;
    }

    @Override
    public Void visitBoundaryDenotation(SmallPearlParser.BoundaryDenotationContext ctx) {
        if (ctx.IntegerConstant().size() == 1 ) {
            ((TypeArray)m_type).addDimension(new ArrayDimension(
                    Defaults.DEFAULT_ARRAY_LWB,
                    Integer.parseInt(ctx.IntegerConstant(0).getText())));
        }
        else {
            ((TypeArray)m_type).addDimension(new ArrayDimension(
                    Integer.parseInt(ctx.IntegerConstant(0).getText()),
                    Integer.parseInt(ctx.IntegerConstant(1).getText())));
        }

        return null;
    }

    @Override
    public Void visitTypeAttributeForArray(SmallPearlParser.TypeAttributeForArrayContext ctx) {
        if ( ctx.type_bit() != null ) {
            TypeDefinition tempType = m_type;
            visitType_bit(ctx.type_bit());
            ((TypeArray)tempType).setBaseType(m_type);
            m_type = tempType;
        }
        else if ( ctx.type_char() != null ) {
            TypeDefinition tempType = m_type;
            visitType_char(ctx.type_char());
            ((TypeArray)tempType).setBaseType(m_type);
            m_type = tempType;
        }
        else if ( ctx.type_clock() != null ) {
            ((TypeArray)m_type).setBaseType(new TypeClock());        }
        else if ( ctx.type_duration() != null ) {
            ((TypeArray)m_type).setBaseType(new TypeDuration());
        }
        else if ( ctx.type_fixed() != null ) {
            TypeDefinition tempType = m_type;
            visitType_fixed(ctx.type_fixed());
            ((TypeArray)tempType).setBaseType(m_type);
            m_type = tempType;
        }
        else if ( ctx.type_float() != null  ) {
            TypeDefinition tempType = m_type;
            visitType_float(ctx.type_float());
            ((TypeArray)tempType).setBaseType(m_type);
            m_type = tempType;
        }
        else if ( ctx.typeReference() != null  ) {
            TypeDefinition tempType = m_type;
            visitTypeReference(ctx.typeReference());
            ((TypeArray)tempType).setBaseType(m_type);
            m_type = tempType;
        }

        return null;
    }

    @Override
    public Void visitType_bit(SmallPearlParser.Type_bitContext ctx) {
        Integer width = Defaults.BIT_LENGTH;

        if (ctx.IntegerConstant() != null) {
            width = Integer.parseInt(ctx.IntegerConstant().getText());
        }

        m_type = new TypeBit(width);
        return null;
    }

    @Override
    public Void visitType_fixed(SmallPearlParser.Type_fixedContext ctx) {
        Integer width = Defaults.FIXED_LENGTH;

        if (ctx.IntegerConstant() != null) {
            width = Integer.parseInt(ctx.IntegerConstant().getText());
            if (width < 1 || width > 63) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        m_type = new TypeFixed(width);
        return null;
    }

    @Override
    public Void visitType_char(SmallPearlParser.Type_charContext ctx) {
        Integer width = Defaults.CHARACTER_LENGTH;

        if (ctx.IntegerConstant() != null) {
            width = Integer.parseInt(ctx.IntegerConstant().getText());
            if (width < 1 || width > 255) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        m_type = new TypeChar(width);
        return null;
    }

    @Override
    public Void visitType_float(SmallPearlParser.Type_floatContext ctx) {
        Integer precision = Defaults.FLOAT_PRECISION;

        if (ctx.IntegerConstant() != null) {
            precision = Integer.parseInt(ctx.IntegerConstant().getText());
        }

        m_type = new TypeFloat(precision);
        return null;
    }

    @Override
    public Void visitSemaDeclaration(SmallPearlParser.SemaDeclarationContext ctx) {
        boolean hasGlobalAttribute = false;

        ArrayList<String> identifierDenotationList = null;
        ArrayList<Integer> presetList = null;

        if (ctx != null) {
            if (ctx.globalAttribute() != null) {
                hasGlobalAttribute = true;
            }

            if (ctx.identifierDenotation() != null) {
                identifierDenotationList = getIdentifierDenotation(ctx.identifierDenotation());
            }
        }

        for (int i = 0; i < identifierDenotationList.size(); i++) {
            SemaphoreEntry entry = new SemaphoreEntry(identifierDenotationList.get(i), ctx);
            if (!m_currentSymbolTable.enter(entry)) {
                System.out.println("ERR: Double definition of " + identifierDenotationList.get(i));
            }
        }

        return null;
    }

    @Override
    public Void visitStatement(SmallPearlParser.StatementContext ctx) {
        if (ctx != null) {
            if ( ctx.label_statement() != null ) {
                for (int i = 0; i < ctx.label_statement().size(); i++) {
                    LabelEntry entry = new LabelEntry(ctx.label_statement(i).ID().getText(), ctx.label_statement(i));

                    if (!m_currentSymbolTable.enter(entry)) {
                        System.out.println("ERR: Double definition of " + ctx.label_statement(i).ID().getText());
                    }
                }
            }

            visitChildren(ctx);
        }

        return null;
    }

    @Override
    public Void visitSemaTry(SmallPearlParser.SemaTryContext ctx) {
        LinkedList<SemaphoreEntry> listOfSemaphores = new LinkedList<SemaphoreEntry>();

        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = symbolTable.lookup(ctx.ID(i).toString());

            if ( entry != null && entry instanceof SemaphoreEntry) {
                listOfSemaphores.add((SemaphoreEntry)entry);
            }
            else {
                throw new ArgumentMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        Collections.sort(listOfSemaphores);
        addToListOfTemporarySemaphoreArrays(listOfSemaphores);

        return null;
    }

    @Override
    public Void visitSemaRequest(SmallPearlParser.SemaRequestContext ctx) {
        LinkedList<SemaphoreEntry> listOfSemaphores = new LinkedList<SemaphoreEntry>();

        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = symbolTable.lookup(ctx.ID(i).toString());

            if ( entry != null && entry instanceof SemaphoreEntry) {
                listOfSemaphores.add((SemaphoreEntry)entry);
            }
            else {
                throw new ArgumentMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        Collections.sort(listOfSemaphores);
        addToListOfTemporarySemaphoreArrays(listOfSemaphores);

        return null;
    }

    @Override
    public Void visitSemaRelease(SmallPearlParser.SemaReleaseContext ctx) {
        LinkedList<SemaphoreEntry> listOfSemaphores = new LinkedList<SemaphoreEntry>();

        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = symbolTable.lookup(ctx.ID(i).toString());

            if ( entry != null && entry instanceof SemaphoreEntry) {
                listOfSemaphores.add((SemaphoreEntry)entry);
            }
            else {
                throw new ArgumentMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        Collections.sort(listOfSemaphores);
        addToListOfTemporarySemaphoreArrays(listOfSemaphores);

        return null;
    }

    private Void addToListOfTemporarySemaphoreArrays( LinkedList<SemaphoreEntry> listOfSemaphores) {
        Boolean found = false;
        for (int i = 0; i < m_listOfTemporarySemaphoreArrays.size(); i++) {
            LinkedList<SemaphoreEntry> semaphores = m_listOfTemporarySemaphoreArrays.get(i);
            if ( semaphores.size() == listOfSemaphores.size()) {
                int j = 0;
                for (j = 0; j < semaphores.size(); j++) {
                    if ( semaphores.get(j).compareTo(listOfSemaphores.get(j)) != 0) {
                        break;
                    }
                }

                if ( j == semaphores.size()) {
                    found = true;
                    break;
                }
            }
        }

        if ( !found) {
            this.m_listOfTemporarySemaphoreArrays.add(listOfSemaphores);
        }

        return null;
    }

    public LinkedList<LinkedList<SemaphoreEntry>>  getListOfTemporarySemaphoreArrays() {
        return m_listOfTemporarySemaphoreArrays;
    }

    private Void addToListOfTemporaryBoltArrays( LinkedList<BoltEntry> listOfBolts) {
        Boolean found = false;
        for (int i = 0; i < m_listOfTemporaryBoltArrays.size(); i++) {
            LinkedList<BoltEntry> bolts = m_listOfTemporaryBoltArrays.get(i);
            if ( bolts.size() == listOfBolts.size()) {
                for (int j = 0; j < bolts.size(); j++) {
                    if ( bolts.get(j).compareTo(listOfBolts.get(j)) == 0) {
                        found = true;
                        break;
                    }
                }
            }
        }

        if ( !found) {
            this.m_listOfTemporaryBoltArrays.add(listOfBolts);
        }

        return null;
    }

    public LinkedList<LinkedList<BoltEntry>>  getListOfTemporaryBoltArrays() {
        return m_listOfTemporaryBoltArrays;
    }

    public LinkedList<ArrayDescriptor>  getListOfArrayDescriptors() {
        return m_listOfArrayDescriptors;
    }

    @Override
    public Void visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        String blockLabel = null;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitLoopStatement");
        }

        LoopEntry loopEntry = new LoopEntry(blockLabel, ctx, m_currentSymbolTable);

        m_currentSymbolTable = m_currentSymbolTable.newLevel(loopEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable );

        if ( ctx.loopStatement_for() != null) {
            VariableEntry controlVariable = new VariableEntry(ctx.loopStatement_for().ID().getText(), new TypeFixed(31), true, null, null);
            controlVariable.setLoopControlVariable();
            m_currentSymbolTable.enter(controlVariable);
        }

        for (int i = 0; i < ctx.scalarVariableDeclaration().size(); i++) {
            visitScalarVariableDeclaration(ctx.scalarVariableDeclaration(i));
        }

        for (int i = 0; i < ctx.arrayVariableDeclaration().size(); i++) {
            visitArrayVariableDeclaration(ctx.arrayVariableDeclaration(i));
        }

        for ( int i = 0; i < ctx.statement().size(); i++) {
            SmallPearlParser.StatementContext stmt = ctx.statement(i);

            if (stmt.block_statement() != null) {
                visitBlock_statement(stmt.block_statement());
            } else if (stmt.unlabeled_statement() != null) {
                visitUnlabeled_statement(stmt.unlabeled_statement());
            }
        }

        if ( ctx.loopStatement_end().ID() != null) {
            blockLabel = ctx.loopStatement_end().ID().getText();
        }

        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitBoltDeclaration( SmallPearlParser.BoltDeclarationContext ctx)
    {
        boolean hasGlobalAttribute = false;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitBoltDeclaration");
        }

        ArrayList<String> identifierDenotationList = null;

        if (ctx != null) {
            if (ctx.globalAttribute() != null) {
                hasGlobalAttribute = true;
            }

            if (ctx.identifierDenotation() != null) {
                identifierDenotationList = getIdentifierDenotation(ctx.identifierDenotation());
            }
        }

        for (int i = 0; i < identifierDenotationList.size(); i++) {
            BoltEntry entry = new BoltEntry(identifierDenotationList.get(i), ctx);
            if (!m_currentSymbolTable.enter(entry)) {
                System.out.println("ERR: Double definition of " + identifierDenotationList.get(i));
            }
        }

        return null;
    }


    @Override
    public Void visitBoltReserve(SmallPearlParser.BoltReserveContext ctx) {
        LinkedList<BoltEntry> listOfBolts = new LinkedList<BoltEntry>();

        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = symbolTable.lookup(ctx.ID(i).toString());

            if ( entry != null && entry instanceof BoltEntry) {
                listOfBolts.add((BoltEntry)entry);
            }
            else {
                throw new ArgumentMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        Collections.sort(listOfBolts);
        addToListOfTemporaryBoltArrays(listOfBolts);

        return null;
    }

    @Override
    public Void visitBoltFree(SmallPearlParser.BoltFreeContext ctx) {
        LinkedList<BoltEntry> listOfBolts = new LinkedList<BoltEntry>();

        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = symbolTable.lookup(ctx.ID(i).toString());

            if ( entry != null && entry instanceof BoltEntry) {
                listOfBolts.add((BoltEntry)entry);
            }
            else {
                throw new ArgumentMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        Collections.sort(listOfBolts);
        addToListOfTemporaryBoltArrays(listOfBolts);

        return null;
    }

    @Override
    public Void visitBoltEnter(SmallPearlParser.BoltEnterContext ctx) {
        LinkedList<BoltEntry> listOfBolts = new LinkedList<BoltEntry>();

        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = symbolTable.lookup(ctx.ID(i).toString());

            if ( entry != null && entry instanceof BoltEntry) {
                listOfBolts.add((BoltEntry)entry);
            }
            else {
                throw new ArgumentMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        Collections.sort(listOfBolts);
        addToListOfTemporaryBoltArrays(listOfBolts);

        return null;
    }

    @Override
    public Void visitBoltLeave(SmallPearlParser.BoltLeaveContext ctx) {
        LinkedList<BoltEntry> listOfBolts = new LinkedList<BoltEntry>();

        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = symbolTable.lookup(ctx.ID(i).toString());

            if ( entry != null && entry instanceof BoltEntry) {
                listOfBolts.add((BoltEntry)entry);
            }
            else {
                throw new ArgumentMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        Collections.sort(listOfBolts);
        addToListOfTemporaryBoltArrays(listOfBolts);

        return null;
    }

    public SymbolTable getSymbolTablePerContext(ParseTree ctx) {
        return m_symboltablePerContext.get(ctx);
    }

    @Override
    public Void visitDationDeclaration(SmallPearlParser.DationDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitDationDeclaration");
        }

        return null;
    }

    @Override
    public Void visitLengthDefinition(SmallPearlParser.LengthDefinitionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitLengthDefinition");
        }

        if ( ctx.lengthDefinitionType() instanceof SmallPearlParser.LengthDefinitionFixedTypeContext) {
            TypeFixed typ = new TypeFixed( Integer.valueOf((ctx.IntegerConstant().toString())));

            if ( typ.getPrecision() < Defaults.FIXED_MIN_LENGTH || typ.getPrecision() > Defaults.FIXED_MAX_LENGTH ) {
                throw new PrecisionNotSupportedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            LengthEntry entry = new LengthEntry(typ,ctx);
            m_currentSymbolTable.enterOrReplace(entry);
        }
        else if ( ctx.lengthDefinitionType() instanceof SmallPearlParser.LengthDefinitionFloatTypeContext) {
            TypeFloat typ = new TypeFloat( Integer.valueOf((ctx.IntegerConstant().toString())));

            if ( typ.getPrecision() != Defaults.FLOAT_SHORT_PRECISION && typ.getPrecision() != Defaults.FLOAT_LONG_PRECISION) {
                throw new PrecisionNotSupportedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            LengthEntry entry = new LengthEntry(typ,ctx);
            m_currentSymbolTable.enterOrReplace(entry);
        }
        else if ( ctx.lengthDefinitionType() instanceof SmallPearlParser.LengthDefinitionBitTypeContext) {
            TypeBit typ = new TypeBit( Integer.valueOf((ctx.IntegerConstant().toString())));
            LengthEntry entry = new LengthEntry(typ,ctx);
            m_currentSymbolTable.enterOrReplace(entry);
        }
        else if ( ctx.lengthDefinitionType() instanceof SmallPearlParser.LengthDefinitionCharacterTypeContext) {
            TypeChar typ = new TypeChar( Integer.valueOf((ctx.IntegerConstant().toString())));
            LengthEntry entry = new LengthEntry(typ,ctx);
            m_currentSymbolTable.enterOrReplace(entry);
        }

        return null;
    }

    private Void addArrayDescriptor(ArrayDescriptor descriptor) {
        boolean found = false;
        for ( int i = 0; i < m_listOfArrayDescriptors.size(); i++) {
            if ( m_listOfArrayDescriptors.get(i).equals(descriptor)) {
                found = true;
            }
        }

        if ( !found) {
            m_listOfArrayDescriptors.add( descriptor);
        }
        return null;
    }


    private ArrayList<Initializer>  getInitialisationAttribute(SmallPearlParser.InitialisationAttributeContext ctx) {
        ArrayList<Initializer> initElementList = new ArrayList<>();

        if (ctx != null) {
            for (int i = 0; i < ctx.initElement().size(); i++) {
                ConstantValue  constant = getInitElement(ctx.initElement(i));
                Initializer initializer = new Initializer(ctx.initElement(i),constant);
                initElementList.add(initializer);
            }
        }

        return initElementList;
    }

    private ConstantValue getInitElement(SmallPearlParser.InitElementContext ctx) {
        ConstantValue constant = null;

        if (ctx.constantExpression() != null) {
            constant = getConstantExpression(ctx.constantExpression());
        } else if (ctx.constant() != null) {
            constant = getConstant(ctx.constant());
        } else if (ctx.ID() != null) {
            String s = ctx.ID().toString();
            SymbolTableEntry entry = this.m_currentSymbolTable.lookup(ctx.ID().toString());

            if ( entry == null ) {
                throw new UnknownIdentifierException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine(),(ctx.ID().toString()));
            }

            if ( entry instanceof VariableEntry) {
                VariableEntry var = (VariableEntry)entry;

                if ( var.getAssigmentProtection()) {
                    constant = var.getInitializer().getConstant();
                }
                else {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine(),(ctx.ID().toString()));
                }
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        m_constantPool.add(constant);
        return constant;
    }

    private ConstantValue getConstant(SmallPearlParser.ConstantContext ctx) {
        ConstantValue constant = null;
        int sign = 1;

        if ( ctx.sign() != null ) {
            if ( ctx.sign() instanceof SmallPearlParser.SignMinusContext ) {
                sign = -1;
            }
        }

        if ( ctx.fixedConstant() != null) {
            int curval = sign * Integer.parseInt(ctx.fixedConstant().IntegerConstant().toString());
            int curlen =   m_currentSymbolTable.lookupDefaultFixedLength();

            if ( ctx.fixedConstant().fixedNumberPrecision() != null ) {
                curlen = Integer.parseInt(ctx.fixedConstant().fixedNumberPrecision().IntegerConstant().toString());
            } else if ( m_type instanceof TypeFixed) {
                curlen = ((TypeFixed)m_type).getPrecision();
            } else if ( m_type instanceof TypeFloat) {
                    curlen = ((TypeFloat)m_type).getPrecision();
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            constant = new ConstantFixedValue(curval,curlen);
        }
        else if ( ctx.floatingPointConstant() != null) {
            double curval = sign * Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
            int curlen =   m_currentSymbolTable.lookupDefaultFloatLength();

            if ( ctx.floatingPointConstant().FloatingPointNumberPrecision() != null ) {
                curlen = Integer.parseInt(ctx.floatingPointConstant().FloatingPointNumberPrecision().toString());
            }else if ( m_type instanceof TypeFloat) {
                curlen = ((TypeFloat)m_type).getPrecision();
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            constant = new ConstantFloatValue(curval,curlen);
        }
        else if ( ctx.durationConstant() != null) {
            Integer hours = 0;
            Integer minutes = 0;
            Double seconds = 0.0;

            if (ctx.durationConstant().hours() != null) {
                hours = Integer.valueOf(ctx.durationConstant().hours().IntegerConstant().toString());
            }
            if (ctx.durationConstant().minutes() != null) {
                minutes = Integer.valueOf(ctx.durationConstant().minutes().IntegerConstant().toString());
            }
            if (ctx.durationConstant().seconds() != null) {
                if (ctx.durationConstant().seconds().IntegerConstant() != null) {
                    seconds = Double.valueOf(ctx.durationConstant().seconds().IntegerConstant().toString());
                } else if (ctx.durationConstant().seconds().floatingPointConstant() != null) {
                    seconds = Double.valueOf(ctx.durationConstant().seconds().floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
                }
            }

            if ( m_type instanceof TypeDuration) {
                constant = new ConstantDurationValue(hours, minutes, seconds);
            }
            else if ( m_type instanceof TypeClock) {
                constant = new ConstantClockValue(hours, minutes, seconds);
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

        }
        else if ( ctx.bitStringConstant() != null) {
            long value = CommonUtils.convertBitStringToLong(ctx.bitStringConstant().BitStringLiteral().getText());
            int  nb = CommonUtils.getBitStringLength(ctx.bitStringConstant().BitStringLiteral().getText());
            constant = new ConstantBitValue(value,nb);
        }
        else if ( ctx.timeConstant() != null) {
            Integer hours = 0;
            Integer minutes = 0;
            Double seconds = 0.0;

            hours = Integer.valueOf(ctx.timeConstant().IntegerConstant(0).toString());
            minutes = Integer.valueOf(ctx.timeConstant().IntegerConstant(1).toString());

            if (ctx.timeConstant().floatingPointConstant() != null) {
                seconds = Double.valueOf(ctx.timeConstant().floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
            }
            else if (ctx.timeConstant().IntegerConstant(2) != null) {
                seconds = Double.valueOf(ctx.timeConstant().IntegerConstant(2).toString());
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            constant = new ConstantClockValue(hours, minutes, seconds);
        }
        else if ( ctx.StringLiteral() != null) {
            constant = new ConstantCharacterValue(ctx.StringLiteral().toString());
        }

        return constant;
    }


    private ConstantValue getConstantExpression(SmallPearlParser.ConstantExpressionContext ctx) {
        ConstantFixedExpressionEvaluator evaluator = new ConstantFixedExpressionEvaluator(m_verbose, m_debug, m_currentSymbolTable,null, null);
        ConstantValue constant = evaluator.visit(ctx.constantFixedExpression());

        return constant;
    }

    @Override
    public Void visitDationSpecification(SmallPearlParser.DationSpecificationContext ctx) {
        LinkedList<ModuleEntry> listOfModules = this.symbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ModuleEntry moduleEntry = listOfModules.get(0);
        SymbolTable symbolTable = moduleEntry.scope;

        symbolTable.setUsesSystemElements();
        return null;
    }

    @Override
    public Void visitStructVariableDeclaration(SmallPearlParser.StructVariableDeclarationContext ctx ) {
        if (m_debug) {
            System.out.println("SymbolTableVisitor: visitStructVariableDeclaration");
        }

        visitChildren(ctx);
        return null;
    }

    @Override
    public Void visitTypeStructure(SmallPearlParser.TypeStructureContext ctx ) {
        if (m_debug) {
            System.out.println("SymbolTableVisitor: visitTypeStructure");
        }

        return null;
    }

    @Override
    public Void visitStructureDenotation(SmallPearlParser.StructureDenotationContext ctx) {
        if (m_debug) {
            System.out.println("SymbolTableVisitor: visitStructureDenotation");
        }

        SymbolTableEntry entry = this.m_currentSymbolTable.lookup(ctx.ID().toString());

        if (entry != null) {
            throw new DoubleDeclarationException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }


        StructureEntry structEntry = new StructureEntry(ctx.ID().getText(), ctx, this.m_currentSymbolTable);
        this.m_currentSymbolTable = this.m_currentSymbolTable.newLevel(structEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable);

        visitChildren(ctx);

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();

        return null;
    }

}
