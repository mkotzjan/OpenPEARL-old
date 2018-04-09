/*
 * [The "BSD license"]
 *  Copyright (c) 2012-2017 Marcel Schaible
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

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.TerminalNodeImpl;
import org.smallpearl.compiler.SymbolTable.*;
import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;
import org.stringtemplate.v4.STGroupFile;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

public class CppCodeGeneratorVisitor extends SmallPearlBaseVisitor<ST> implements SmallPearlVisitor<ST> {

    private STGroup m_group;
    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;
    private ExpressionTypeVisitor m_expressionTypeVisitor;
    private ConstantExpressionEvaluatorVisitor m_constantExpressionEvaluatorVisitor;
    private SymbolTableVisitor m_symbolTableVisitor;
    private boolean m_map_to_const = true;
    private SymbolTable m_symboltable;
    private SymbolTable m_currentSymbolTable;
    private ModuleEntry m_module;
    private Integer m_currFixedLength = null;

    public enum Type {BIT, CHAR, FIXED}

    public static final double pi = java.lang.Math.PI;

    public CppCodeGeneratorVisitor(String sourceFileName,
                                   String filename,
                                   int verbose,
                                   boolean debug,
                                   SymbolTableVisitor symbolTableVisitor,
                                   ExpressionTypeVisitor expressionTypeVisitor,
                                   ConstantExpressionEvaluatorVisitor constantExpressionEvaluatorVisitor) {

        m_debug = debug;
        m_verbose = verbose;
        m_sourceFileName = sourceFileName;
        m_symbolTableVisitor = symbolTableVisitor;
        m_expressionTypeVisitor = expressionTypeVisitor;
        m_constantExpressionEvaluatorVisitor = constantExpressionEvaluatorVisitor;
        m_symboltable = symbolTableVisitor.symbolTable;
        m_currentSymbolTable = m_symboltable;

        m_verbose = 1;

        LinkedList<ModuleEntry> listOfModules = this.m_currentSymbolTable.getModules();

        if ( listOfModules.size() > 1 ) {
            throw new NotYetImplementedException("Multiple modules", 0, 0);
        }

        m_module = listOfModules.get(0);

        if (m_verbose > 0) {
            System.out.println("Generating Cpp code");
        }

        this.ReadTemplate(filename);
        generatePrologue();
    }

    private Void ReadTemplate(String filename) {
        if (m_verbose > 0) {
            System.out.println("Read StringTemplate Group File: " + filename);
        }

        this.m_group = new STGroupFile(filename);

        return null;
    }

    private ST generatePrologue() {
        ST prologue = m_group.getInstanceOf("Prologue");

        prologue.add("src", this.m_sourceFileName);

        ST taskspec = m_group.getInstanceOf("TaskSpecifier");

        LinkedList<TaskEntry> taskEntries = this.m_module.scope.getTaskDeclarations();
        ArrayList<String> listOfTaskNames = new ArrayList<String>();

        for (int i = 0; i < taskEntries.size(); i++) {
            listOfTaskNames.add(taskEntries.get(i).getName());
        }

        taskspec.add("taskname", listOfTaskNames);
        prologue.add("taskSpecifierList", taskspec);
        prologue.add("ConstantPoolList", generateConstantPool());

        if (m_module.scope.usesSystemElements()) {
            prologue.add("useSystemElements", true);
        }

        return prologue;
    }

    private ST generateConstantPool() {
        ST pool = m_group.getInstanceOf("ConstantPoolList");

        for (int i = 0; i < ConstantPool.constantPool.size(); i++) {
            if (ConstantPool.constantPool.get(i) instanceof ConstantFixedValue ) {
                ST entry = m_group.getInstanceOf("ConstantPoolEntry");
                entry.add("name", ((ConstantFixedValue) ConstantPool.constantPool.get(i)).toString());
                entry.add("type", ((ConstantFixedValue) ConstantPool.constantPool.get(i)).getBaseType());
                entry.add("precision", ((ConstantFixedValue) ConstantPool.constantPool.get(i)).getPrecision());
                entry.add("value", ((ConstantFixedValue) ConstantPool.constantPool.get(i)).getValue());
                pool.add("constants", entry);
            }
        }

        for (int i = 0; i < ConstantPool.constantPool.size(); i++) {
            if (ConstantPool.constantPool.get(i) instanceof ConstantFloatValue) {
                ST entry = m_group.getInstanceOf("ConstantPoolEntry");
                entry.add("name", ((ConstantFloatValue) ConstantPool.constantPool.get(i)).toString());
                entry.add("type", ((ConstantFloatValue) ConstantPool.constantPool.get(i)).getBaseType());
                entry.add("precision", ((ConstantFloatValue) ConstantPool.constantPool.get(i)).getPrecision());
                entry.add("value", ((ConstantFloatValue) ConstantPool.constantPool.get(i)).getValue());
                pool.add("constants", entry);
            }
        }

        for (int i = 0; i < ConstantPool.constantPool.size(); i++) {
            if (ConstantPool.constantPool.get(i) instanceof ConstantCharacterValue) {
                ConstantCharacterValue value = (ConstantCharacterValue) ConstantPool.constantPool.get(i);
                ST entry = m_group.getInstanceOf("ConstantPoolCharacterEntry");
                entry.add("name", value.toString());
                entry.add("type", value.getBaseType());

                String s = CommonUtils.unescapePearlString(value.getValue());
                entry.add("length", CommonUtils.getStringLength(s));
                entry.add("value", s);

                pool.add("constants", entry);
            }
        }

        for (int i = 0; i < ConstantPool.constantPool.size(); i++) {
            if (ConstantPool.constantPool.get(i) instanceof ConstantBitValue) {
                ConstantBitValue value = (ConstantBitValue) ConstantPool.constantPool.get(i);
                ST entry = m_group.getInstanceOf("ConstantPoolBitEntry");
                entry.add("name", value.toString());
                entry.add("type", value.getBaseType());
                entry.add("length", value.getLength());
                entry.add("value", value.getValue());
                pool.add("constants", entry);
            }
        }

        for (int i = 0; i < ConstantPool.constantPool.size(); i++) {
            if (ConstantPool.constantPool.get(i) instanceof ConstantDurationValue) {
                ConstantDurationValue value = (ConstantDurationValue) ConstantPool.constantPool.get(i);
                ST entry = m_group.getInstanceOf("ConstantPoolDurationEntry");
                entry.add("name", value.toString());
                entry.add("type", value.getBaseType());
                entry.add("value", value.getValue());

                pool.add("constants", entry);
            }
        }

        for (int i = 0; i < ConstantPool.constantPool.size(); i++) {
            if (ConstantPool.constantPool.get(i) instanceof ConstantClockValue) {
                ConstantClockValue value = (ConstantClockValue) ConstantPool.constantPool.get(i);
                ST entry = m_group.getInstanceOf("ConstantPoolClockEntry");
                entry.add("name", value.toString());
                entry.add("type", value.getBaseType());
                entry.add("value", value.getValue());

                pool.add("constants", entry);
            }
        }

        return pool;
    }

    private double getDuration(SmallPearlParser.DurationConstantContext ctx) {
        Integer hours = 0;
        Integer minutes = 0;
        Double seconds = 0.0;

        if (ctx.hours() != null) {
            hours = Integer.valueOf(ctx.hours().IntegerConstant().toString()) * 3600;
        }
        if (ctx.minutes() != null) {
            minutes = Integer.valueOf(ctx.minutes().IntegerConstant().toString()) * 60;
        }
        if (ctx.seconds() != null) {
            if (ctx.seconds().IntegerConstant() != null) {
                seconds = Double.valueOf(ctx.seconds().IntegerConstant().toString());
            } else if (ctx.seconds().floatingPointConstant() != null) {
                seconds = Double.valueOf(ctx.seconds().floatingPointConstant().FloatingPointNumberPrecision().getText());
            }
        }

        return (hours + minutes + seconds);
    }

    private ConstantDurationValue getConstantDurationValue(SmallPearlParser.DurationConstantContext ctx) {
        int hours = 0;
        int minutes = 0;
        double seconds = 0.0;

        if (ctx.hours() != null) {
            hours = Integer.valueOf(ctx.hours().IntegerConstant().toString());
        }
        if (ctx.minutes() != null) {
            minutes = Integer.valueOf(ctx.minutes().IntegerConstant().toString());
        }
        if (ctx.seconds() != null) {
            if (ctx.seconds().IntegerConstant() != null) {
                seconds = Double.valueOf(ctx.seconds().IntegerConstant().toString());
            } else if (ctx.seconds().floatingPointConstant() != null) {
                seconds = Double.valueOf(ctx.seconds().floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
            }
        }

        return new ConstantDurationValue(hours,minutes,seconds);
    }

    /*
    Time/Clock example:
           11:30:00    means 11.30
           15:45:3.5   means 15.45 and 3.5 seconds
           25:00:00    means 1.00
    */
    private Double getTime(SmallPearlParser.TimeConstantContext ctx) {
        Integer hours = 0;
        Integer minutes = 0;
        Double seconds = 0.0;

        hours = (Integer.valueOf(ctx.IntegerConstant(0).toString()) % 24);
        minutes = Integer.valueOf(ctx.IntegerConstant(1).toString());

        if (ctx.IntegerConstant().size() == 3) {
            seconds = Double.valueOf(ctx.IntegerConstant(2).toString());
        }

        if ( ctx.floatingPointConstant() != null ) {
            seconds = Double.valueOf(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
        }

        if (hours < 0 || minutes < 0 || minutes > 59) {
            throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return hours * 3600 + minutes * 60 + seconds;
    }

    @Override
    public ST visitModule(SmallPearlParser.ModuleContext ctx) {
        ST module = m_group.getInstanceOf("module");

        module.add("src", this.m_sourceFileName);
        module.add("name", ctx.ID().getText());
        module.add("prologue", generatePrologue());

        org.smallpearl.compiler.SymbolTable.SymbolTableEntry symbolTableEntry = m_currentSymbolTable.lookupLocal(ctx.ID().getText());
        m_currentSymbolTable = ((org.smallpearl.compiler.SymbolTable.ModuleEntry)symbolTableEntry).scope;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.System_partContext) {
                    module.add("SystemPart", visitSystem_part((SmallPearlParser.System_partContext) c));
                } else if (c instanceof SmallPearlParser.Problem_partContext) {
                    module.add("ProblemPart", visitProblem_part((SmallPearlParser.Problem_partContext) c));
                } else if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                    ST decl = m_group.getInstanceOf("cpp_inline");
                    module.add("cpp_inlines", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                }
            }
        }

        m_currentSymbolTable = m_currentSymbolTable.ascend();

        return module;
    }

    @Override
    public ST visitSystem_part(SmallPearlParser.System_partContext ctx) {
        ST st = m_group.getInstanceOf("SystemPart");

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                    ST decl = m_group.getInstanceOf("cpp_inline");
                    st.add("cpp_inlines", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                } else if (c instanceof SmallPearlParser.Username_declarationContext) {
                    visitUsername_declaration((SmallPearlParser.Username_declarationContext) c);
                } else if (c instanceof SmallPearlParser.UserConfigurationWithoutAssociationContext) {
                    visitUserConfigurationWithoutAssociation((SmallPearlParser.UserConfigurationWithoutAssociationContext) c);
                } else if (c instanceof SmallPearlParser.UserConfigurationWithAssociationContext) {
                    visitUserConfigurationWithAssociation((SmallPearlParser.UserConfigurationWithAssociationContext) c);
                }
            }
        }

        return st;
    }

    @Override
    public ST visitType(SmallPearlParser.TypeContext ctx) {
        ST type = m_group.getInstanceOf("type");

        if (ctx.simple_type() != null) {
            type.add("simple_type", visitSimple_type(ctx.simple_type()));
        } else if (ctx.typeTime() != null) {
            type.add("TypeTime", visitTypeTime(ctx.typeTime()));
        }

        return type;
    }

    @Override
    public ST visitSimple_type(SmallPearlParser.Simple_typeContext ctx) {
        ST simple_type = m_group.getInstanceOf("simple_type");

        if (ctx.type_fixed() != null) {
            simple_type.add("type_fixed", visitType_fixed(ctx.type_fixed()));
        } else if (ctx.type_char() != null) {
            simple_type.add("type_char", visitType_char(ctx.type_char()));
        } else if (ctx.type_float() != null) {
            simple_type.add("type_float", visitType_float(ctx.type_float()));
        }

        return simple_type;
    }


    @Override
    public ST visitTypeReferenceTaskType(SmallPearlParser.TypeReferenceTaskTypeContext ctx) {
        ST st = m_group.getInstanceOf("TypeReferenceTaskType");
        return st;
    }

    @Override
    public ST visitTypeReference(SmallPearlParser.TypeReferenceContext ctx) {
        return visitChildren(ctx);
    }

    @Override
    public ST visitTypeReferences(SmallPearlParser.TypeReferencesContext ctx) {
        return visitChildren(ctx);
    }

    @Override
    public ST visitTypeReferenceSimpleType(SmallPearlParser.TypeReferenceSimpleTypeContext ctx) {
        ST st = m_group.getInstanceOf("TypeReferenceSimpleType");

        if ( ctx.simpleType() != null ) {
            st.add("BaseType", visitSimpleType(ctx.simpleType()));
        }

        return st;
    }

    @Override
    public ST visitTypeReferenceProcedureType(SmallPearlParser.TypeReferenceProcedureTypeContext ctx) {
//        ST st = m_group.getInstanceOf("TypeReferenceProcedure");
//        return st;
        throw new NotYetImplementedException("REF PROC", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public ST visitTypeReferenceSemaType(SmallPearlParser.TypeReferenceSemaTypeContext ctx) {
//        ST st = m_group.getInstanceOf("TypeReferenceSema");
//        return st;
        throw new NotYetImplementedException("REF SEMA", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public ST visitTypeReferenceBoltType(SmallPearlParser.TypeReferenceBoltTypeContext ctx) {
//        ST st = m_group.getInstanceOf("TypeReferenceBolt");
//        return st;
        throw new NotYetImplementedException("REF BOLT", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public ST visitTypeReferenceCharType(SmallPearlParser.TypeReferenceCharTypeContext ctx) {
//        ST st = m_group.getInstanceOf("TypeReferenceChar");
//        return st;
        throw new NotYetImplementedException("REF CHAR", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public ST visitTypeReferenceSignalType(SmallPearlParser.TypeReferenceSignalTypeContext ctx) {
//        ST st = m_group.getInstanceOf("TypeReferenceSignal");
//        return st;
        throw new NotYetImplementedException("REF SIGNAL", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public ST visitTypeReferenceInterruptType(SmallPearlParser.TypeReferenceInterruptTypeContext ctx) {
//        ST st = m_group.getInstanceOf("TypeReferenceInterrupt");
//        return st;
        throw new NotYetImplementedException("REF INTERRUPT", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public ST visitType_fixed(SmallPearlParser.Type_fixedContext ctx) {
        ST fixed_type = m_group.getInstanceOf("fixed_type");
        int width = Defaults.FIXED_LENGTH;

        if (ctx.IntegerConstant() != null) {
            width = Integer.parseInt(ctx.IntegerConstant().getText());
            if (width < 1 || width > 63) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        fixed_type.add("size", width);

        return fixed_type;
    }

    @Override
    public ST visitType_char(SmallPearlParser.Type_charContext ctx) {
        ST char_type = m_group.getInstanceOf("char_type");
        Integer width = Defaults.CHARACTER_LENGTH;

        if (ctx.IntegerConstant() != null) {
            width = Integer.parseInt(ctx.IntegerConstant().getText());
        }

        char_type.add("size", width);

        return char_type;
    }

    @Override
    public ST visitType_float(SmallPearlParser.Type_floatContext ctx) {
        ST float_type = m_group.getInstanceOf("float_type");
        int precision = m_currentSymbolTable.lookupDefaultFloatLength();

        if (ctx.IntegerConstant() != null) {
            precision = Integer.parseInt(ctx.IntegerConstant().getText());
        }

        float_type.add("size", precision);
        return float_type;
    }

    @Override
    public ST visitTypeTime(SmallPearlParser.TypeTimeContext ctx) {
        ST time_type = m_group.getInstanceOf("time_type");

        if (ctx.type_clock() != null) {
            time_type.add("clock_type", visitType_clock(ctx.type_clock()));
        } else if (ctx.type_duration() != null) {
            time_type.add("duration_type", visitType_duration(ctx.type_duration()));
        }

        return time_type;
    }

    @Override
    public ST visitType_clock(SmallPearlParser.Type_clockContext ctx) {
        ST type = m_group.getInstanceOf("clock_type");

        type.add("init", 0);
        return type;
    }

    @Override
    public ST visitType_duration(SmallPearlParser.Type_durationContext ctx) {
        ST type = m_group.getInstanceOf("duration_type");
        type.add("init", 0);
        return type;
    }

    @Override
    public ST visitScalarVariableDeclaration(SmallPearlParser.ScalarVariableDeclarationContext ctx) {
        ST scalarVariableDeclaration = m_group.getInstanceOf("ScalarVariableDeclaration");

        if (ctx != null) {
            for (int i = 0; i < ctx.variableDenotation().size(); i++) {
                scalarVariableDeclaration.add("variable_denotations", visitVariableDenotation(ctx.variableDenotation().get(i)));
            }

            if (ctx.cpp_inline() != null) {
                scalarVariableDeclaration.add("cpp", visitCpp_inline(ctx.cpp_inline()));
            }
        }

        return scalarVariableDeclaration;
    }

    @Override
    public ST visitVariableDenotation(SmallPearlParser.VariableDenotationContext ctx) {
        ST variableDenotation = m_group.getInstanceOf("variable_denotation");
        ST typeAttribute = m_group.getInstanceOf("TypeAttribute");
        ArrayList<String> identifierDenotationList = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.IdentifierDenotationContext) {
                    identifierDenotationList = getIdentifierDenotation((SmallPearlParser.IdentifierDenotationContext) c);
                }
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                ST v = m_group.getInstanceOf("VariableDeclaration");

                SymbolTableEntry entry = m_currentSymbolTable.lookup(identifierDenotationList.get(i));
                VariableEntry var = (VariableEntry)entry;

                v.add("name", identifierDenotationList.get(i));
                v.add("TypeAttribute", var.getType().toST(m_group));
//                v.add("global", "?");
                v.add("inv", var.getAssigmentProtection());

                if (var.getInitializer() != null) {
                    v.add("InitElement", var.getInitializer().getConstant());
                }

                variableDenotation.add("decl", v);
            }
        }

        return variableDenotation;
    }

/*
    @Override
    public ST visitVariableDenotation(SmallPearlParser.VariableDenotationContext ctx) {
        ST variableDenotation = m_group.getInstanceOf("variable_denotation");
        ST typeAttribute = m_group.getInstanceOf("TypeAttribute");
        boolean hasGlobalAttribute = false;
        boolean hasAllocationProtection = false;

        ArrayList<String> identifierDenotationList = null;
        ArrayList<ST> initElementList = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.IdentifierDenotationContext) {
                    identifierDenotationList = getIdentifierDenotation((SmallPearlParser.IdentifierDenotationContext) c);
                } else if (c instanceof SmallPearlParser.AllocationProtectionContext) {
                    hasAllocationProtection = true;
                } else if (c instanceof SmallPearlParser.TypeAttributeContext) {
                    typeAttribute = visitTypeAttribute((SmallPearlParser.TypeAttributeContext) c);
                } else if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                    hasGlobalAttribute = true;
                } else if (c instanceof SmallPearlParser.InitialisationAttributeContext) {
                    initElementList = getInitialisationAttribute((SmallPearlParser.InitialisationAttributeContext) c);
                }
            }

            if (initElementList != null && identifierDenotationList.size() != initElementList.size()) {
                throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                ST v = m_group.getInstanceOf("VariableDeclaration");
                v.add("name", identifierDenotationList.get(i));
                v.add("TypeAttribute", typeAttribute);
                v.add("global", hasGlobalAttribute);
                v.add("inv", hasAllocationProtection);

                if (initElementList != null)
                    v.add("InitElement", initElementList.get(i));

                variableDenotation.add("decl", v);
            }
        }

        return variableDenotation;
    }
*/

    private ArrayList<String> getIdentifierDenotation(SmallPearlParser.IdentifierDenotationContext ctx) {
        ArrayList<String> identifierDenotationList = new ArrayList<String>();

        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                identifierDenotationList.add(ctx.ID().get(i).toString());
            }
        }

        return identifierDenotationList;
    }

    private ArrayList<Integer> getPreset(SmallPearlParser.PresetContext ctx) {
        ArrayList<Integer> presetList = new ArrayList<Integer>();

        if (ctx != null) {
            for (int i = 0; i < ctx.integerWithoutPrecision().size(); i++) {
                Integer preset = Integer.parseInt(ctx.integerWithoutPrecision(i).IntegerConstant().getText());
                presetList.add(preset);
            }
        }

        return presetList;
    }


    private ST getArrayInitialisationAttribute(SmallPearlParser.ArrayInitialisationAttributeContext ctx, int noOfElements) {
        ST st = m_group.getInstanceOf("ArrayInitalisations");
        ST last_value = null;

        if ( noOfElements < ctx.initElement().size()) {
            throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        for (int i = 0; i < noOfElements; i++) {
            ST element = m_group.getInstanceOf("InitElement");

            if ( i < ctx.initElement().size()) {
                ConstantValue value = m_constantExpressionEvaluatorVisitor.lookup((ctx.initElement().get(i).constantExpression()));

                ST stValue = m_group.getInstanceOf("expression");
                stValue.add("code", value);
                last_value = stValue;
            }

            st.add("initelements", last_value);
        }

        return st;
    }

    private ArrayList<ST> getInitialisationAttribute(SmallPearlParser.InitialisationAttributeContext ctx) {
        ArrayList<ST> initElementList = new ArrayList<ST>();

        if (ctx != null) {
            for (int i = 0; i < ctx.initElement().size(); i++) {

                if ( ctx.initElement(i).constantExpression() != null ) {
                    ConstantValue value = m_constantExpressionEvaluatorVisitor.lookup((ctx.initElement().get(i).constantExpression()));

                    ST stValue = m_group.getInstanceOf("expression");

                    if (value instanceof ConstantFixedValue) {
                        stValue.add("code", ((ConstantFixedValue) value).toString());
                    } else {
                        stValue.add("code", value);
                    }

                    initElementList.add(stValue);
                } else if ( ctx.initElement(i).constant() != null ) {
                    ST stValue = m_group.getInstanceOf("expression");
                    stValue.add("code", getInitElement(ctx.initElement(i).constant()));
                    initElementList.add(stValue);
                } else if ( ctx.initElement(i).ID() != null ) {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
        }

        return initElementList;
    }

    private ST getInitElement(SmallPearlParser.ConstantContext ctx) {
        ST constant = m_group.getInstanceOf("Constant");

        if (ctx != null) {
            if (ctx.fixedConstant() != null) {
                ST integerConstant = m_group.getInstanceOf("IntegerConstant");

                ConstantValue value = m_constantExpressionEvaluatorVisitor.lookup(ctx.fixedConstant());

                System.out.println("#####"+ctx.fixedConstant().IntegerConstant().toString());

                if ( value == null ) {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                ST expr = m_group.getInstanceOf("expression");

                if (value instanceof ConstantFixedValue) {
                    expr.add("code", ((ConstantFixedValue) value).getValue());
                } else {
                    expr.add("code", value);
                }

                integerConstant.add("value", expr);
                constant.add("IntegerConstant",integerConstant);
            } else if (ctx.durationConstant() != null) {
                ST durationConstant = m_group.getInstanceOf("DurationConstant");
                durationConstant.add("value", visitDurationConstant(ctx.durationConstant()));
                constant.add("DurationConstant", durationConstant);
            } else if (ctx.timeConstant() != null) {
                ST timeConstant = m_group.getInstanceOf("TimeConstant");
                timeConstant.add("value", visitTimeConstant(ctx.timeConstant()));
                constant.add("TimeConstant", timeConstant);
            } else if (ctx.floatingPointConstant() != null) {
                ST durationConstant = m_group.getInstanceOf("FloatingPointConstant");
                Double value;
                Integer sign = 1;

                value = Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().getText());

                if (ctx.getChildCount() > 1) {
                    if (ctx.getChild(0).getText().equals("-")) {
                        value = -value;
                    }
                }

                constant.add("FloatingPointConstant", value);
            } else if (ctx.StringLiteral() != null) {
                ST stringConstant = m_group.getInstanceOf("StringConstant");
                String s = ctx.StringLiteral().toString();

                if (s.startsWith("'")) {
                    s = s.substring(1, s.length());
                }

                if (s.endsWith("'")) {
                    s = s.substring(0, s.length() - 1);
                }

                s = CommonUtils.unescapePearlString(s);

                stringConstant.add("value", s);
                constant.add("StringConstant", stringConstant);
            } else if (ctx.bitStringConstant() != null) {
                constant.add("BitStringConstant", getBitStringConstant(ctx));
            }
        }

        return constant;
    }


/*
    private ST getInitElement(SmallPearlParser.ConstantContext ctx) {
        ST constant = m_group.getInstanceOf("Constant");

        if (ctx != null) {
            if (ctx.fixedConstant() != null) {
                ST integerConstant = m_group.getInstanceOf("IntegerConstant");
                int value;
                int sign = 1;
                int precision = Defaults.FIXED_LENGTH;

                value = Integer.parseInt(ctx.fixedConstant().IntegerConstant().getText());

                if (ctx.getChildCount() > 1) {
                    if (ctx.getChild(0).getText().equals("-")) {
                        value = -value;
                    }
                }


                if ( ctx.fixedConstant().IntegerConstant() != null ) {
                    ConstantValue nvalue = m_constantExpressionEvaluatorVisitor.lookup(ctx.fixedConstant());

                    ST stValue = m_group.getInstanceOf("expression");

                    if (nvalue instanceof ConstantFixedValue) {
                        stValue.add("code", ((ConstantFixedValue) nvalue).getValue());
                    } else {
                        stValue.add("code", nvalue);
                    }
                } else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }


                if (ctx.fixedConstant().fixedNumberPrecision() != null ) {
                    precision = Integer.parseInt(ctx.fixedConstant().fixedNumberPrecision().IntegerConstant().toString());
                }
                else {
                    // walk up the AST and get VariableDenotationContext:
                    ParserRuleContext sctx = ctx.getParent();
                    while (sctx != null && !(sctx instanceof SmallPearlParser.VariableDenotationContext)) {
                        sctx = sctx.getParent();
                    }

                    if (sctx != null) {
                        SmallPearlParser.TypeAttributeContext typeAttributeContext = ((SmallPearlParser.VariableDenotationContext) sctx).typeAttribute();
                        if (typeAttributeContext.simpleType() != null) {
                            SmallPearlParser.SimpleTypeContext simpleTypeContext = typeAttributeContext.simpleType();

                            if (simpleTypeContext.typeInteger() != null) {
                                SmallPearlParser.TypeIntegerContext typeIntegerContext = simpleTypeContext.typeInteger();

                                if (typeIntegerContext.mprecision() != null) {
                                    precision = Integer.parseInt(typeIntegerContext.mprecision().integerWithoutPrecision().IntegerConstant().toString());
                                }
                            }
                        }

                    } else {
                        throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                    }
                }

                ConstantFixedValue fixedConst = ConstantPool.lookupFixedValue(value,precision);

                if ( fixedConst != null ) {
                    integerConstant.add("value", fixedConst.toString());
                }
                else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

*/
/*
                if (Integer.toBinaryString(Math.abs(value)).length() < 31) {
                    integerConstant.add("value", value);
                } else {
                    integerConstant.add("value", value + "LL");
                }
*//*


                constant.add("IntegerConstant", integerConstant);
            } else if (ctx.durationConstant() != null) {
                ST durationConstant = m_group.getInstanceOf("DurationConstant");
                durationConstant.add("value", visitDurationConstant(ctx.durationConstant()));
                constant.add("DurationConstant", durationConstant);
            } else if (ctx.timeConstant() != null) {
                ST timeConstant = m_group.getInstanceOf("TimeConstant");
                timeConstant.add("value", visitTimeConstant(ctx.timeConstant()));
                constant.add("TimeConstant", timeConstant);
            } else if (ctx.floatingPointConstant() != null) {
                ST durationConstant = m_group.getInstanceOf("FloatingPointConstant");
                Double value;
                Integer sign = 1;

                value = Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().getText());

                if (ctx.getChildCount() > 1) {
                    if (ctx.getChild(0).getText().equals("-")) {
                        value = -value;
                    }
                }

                constant.add("FloatingPointConstant", value);
            } else if (ctx.StringLiteral() != null) {
                ST stringConstant = m_group.getInstanceOf("StringConstant");
                String s = ctx.StringLiteral().toString();

                if (s.startsWith("'")) {
                    s = s.substring(1, s.length());
                }

                if (s.endsWith("'")) {
                    s = s.substring(0, s.length() - 1);
                }

                s = CommonUtils.unescapePearlString(s);

                stringConstant.add("value", s);
                constant.add("StringConstant", stringConstant);
            } else if (ctx.bitStringConstant() != null) {
                constant.add("BitStringConstant", getBitStringConstant(ctx));
            }
        }

        return constant;
    }
*/

    private String getBitStringConstant(SmallPearlParser.ConstantContext ctx) {
        String bitString = ctx.bitStringConstant().BitStringLiteral().toString();
        int nb = CommonUtils.getBitStringLength(bitString);
        long value = CommonUtils.convertBitStringToLong(bitString);

        ConstantBitValue bitConst = ConstantPool.lookupBitValue(value,nb);

        if ( bitConst != null) {
            return bitConst.toString();
        }

        throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    private ST formatBitStringConstant(Long l, int numberOfBits) {
        ST bitStringConstant = m_group.getInstanceOf("BitStringConstant");
        ST constant = m_group.getInstanceOf("Constant");

        String b = Long.toBinaryString(l);
        String bres = "";

        int l1 = b.length();

        if (numberOfBits < b.length())

        {
            bres = "";
            for (int i = 0; i < numberOfBits; i++) {
                bres = bres + b.charAt(i);
            }

            Long r = Long.parseLong(bres, 2);

            if (Long.toBinaryString(Math.abs(r)).length() < 15) {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(r).toString());
            } else if (Long.toBinaryString(Math.abs(r)).length() < 31) {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(r).toString() + "UL");
            } else {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(r).toString() + "ULL");
            }
        } else if (numberOfBits > b.length()) {
            bres = b;
            Long r = Long.parseLong(bres, 2);
            if (Long.toBinaryString(Math.abs(r)).length() < 15) {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(r).toString());
            } else if (Long.toBinaryString(Math.abs(r)).length() < 31) {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(r).toString() + "UL");
            } else {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(r).toString() + "ULL");
            }
        } else
        {
            if (Long.toBinaryString(Math.abs(l)).length() < 15) {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(l).toString());
            } else if (Long.toBinaryString(Math.abs(l)).length() < 31) {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(l).toString() + "UL");
            } else {
                bitStringConstant.add("value", "## 0x" + Long.toHexString(l).toString() + "ULL");
            }
        }

        bitStringConstant.add("length", b.length());

        constant.add("BitStringConstant", bitStringConstant);

        return constant;
    }

    @Override
    public ST visitDurationConstant(SmallPearlParser.DurationConstantContext ctx) {
        ST st = m_group.getInstanceOf("DurationConstant");
        Integer hours = 0;
        Integer minutes = 0;
        Double seconds = 0.0;

        if (ctx.hours() != null) {
            hours = getHours(ctx.hours());
        }

        if (ctx.minutes() != null) {
            minutes = getMinutes(ctx.minutes());
        }

        if (ctx.seconds() != null) {
            seconds = getSeconds(ctx.seconds());
        }

        ConstantDurationValue constant = ConstantPool.lookupDurationValue(hours,minutes,seconds);

        if ( constant == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        st.add("value", constant);
        return st;
    }

    private Integer getHours(SmallPearlParser.HoursContext ctx) {
        Integer hours = 0;

        if (ctx.IntegerConstant() != null) {
            hours = Integer.parseInt(ctx.IntegerConstant().getText());
        }

        return hours;
    }

    private Integer getMinutes(SmallPearlParser.MinutesContext ctx) {
        Integer minutes = 0;

        if (ctx.IntegerConstant() != null) {
            minutes = Integer.parseInt(ctx.IntegerConstant().getText());
        }

        return minutes;
    }

    private Double getSeconds(SmallPearlParser.SecondsContext ctx) {
        Double seconds = 0.0;

        if (ctx.IntegerConstant() != null) {
            seconds = (double) Integer.parseInt(ctx.IntegerConstant().getText());
            if (seconds < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.floatingPointConstant() != null) {
            Integer sign = 1;

            seconds = Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().getText());

            if (seconds < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return seconds;
    }

    @Override
    public ST visitTypeAttributeForArray(SmallPearlParser.TypeAttributeForArrayContext ctx) {
        ST type = m_group.getInstanceOf("TypeAttribute");

        if (ctx.type_fixed() != null) {
            type.add("Type", visitType_fixed(ctx.type_fixed()));
        }

        return type;
    }

    @Override
    public ST visitTypeAttribute(SmallPearlParser.TypeAttributeContext ctx) {
        ST type = m_group.getInstanceOf("TypeAttribute");

        if (ctx.simpleType() != null) {
            type.add("Type", visitSimpleType(ctx.simpleType()));
        }
        else if (ctx.typeReference()!= null) {
            type.add("Type", visitTypeReference(ctx.typeReference()));
        }

        return type;
    }

    @Override
    public ST visitSimpleType(SmallPearlParser.SimpleTypeContext ctx) {
        ST simpleType = m_group.getInstanceOf("SimpleType");

        if (ctx != null) {
            if (ctx.typeInteger() != null) {
                simpleType.add("TypeInteger", visitTypeInteger(ctx.typeInteger()));
            } else if (ctx.typeDuration() != null) {
                simpleType.add("TypeDuration", visitTypeDuration(ctx.typeDuration()));
            } else if (ctx.typeBitString() != null) {
                simpleType.add("TypeBitString", visitTypeBitString(ctx.typeBitString()));
            } else if (ctx.typeFloatingPointNumber() != null) {
                simpleType.add("TypeFloatingPointNumber", visitTypeFloatingPointNumber(ctx.typeFloatingPointNumber()));
            } else if (ctx.typeTime() != null) {
                simpleType.add("TypeTime", visitTypeTime(ctx.typeTime()));
            } else if (ctx.typeCharacterString() != null) {
            // TODO: TypeCharacterString
                simpleType.add("TypeCharacterString", visitTypeCharacterString(ctx.typeCharacterString()));
            }
        }

        return simpleType;
    }

    @Override
    public ST visitTypeDuration(SmallPearlParser.TypeDurationContext ctx) {
        ST st = m_group.getInstanceOf("TypeDuration");
        Integer size = 31;

        if (ctx != null) {
            st.add("code", 1);
        }

        return st;
    }

    @Override
    public ST visitTypeInteger(SmallPearlParser.TypeIntegerContext ctx) {
        ST st = m_group.getInstanceOf("TypeInteger");
        int size = m_currentSymbolTable.lookupDefaultFixedLength();

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.MprecisionContext) {
                    size = Integer.parseInt(((SmallPearlParser.MprecisionContext) c).integerWithoutPrecision().IntegerConstant().getText());
                }
            }
        }

        st.add("size", size);

        return st;
    }

    @Override
    public ST visitTypeBitString(SmallPearlParser.TypeBitStringContext ctx) {
        ST st = m_group.getInstanceOf("TypeBitString");

        int length = m_currentSymbolTable.lookupDefaultBitLength();

        if (ctx.IntegerConstant() != null) {
            length = Integer.parseInt(ctx.IntegerConstant().getText());
            if (length < 1 || length > 64) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        st.add("length", length);

        return st;

    }

    @Override
    public ST visitTypeCharacterString(SmallPearlParser.TypeCharacterStringContext ctx) {
        ST st = m_group.getInstanceOf("TypeCharacterString");
        Integer size = Defaults.CHARACTER_LENGTH;

        if (ctx.IntegerConstant() != null) {
            size = Integer.parseInt(ctx.IntegerConstant().getText());

            if (size < 1 || size > 255) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        st.add("size", size);

        return st;
    }

    @Override
    public ST visitTypeFloatingPointNumber(SmallPearlParser.TypeFloatingPointNumberContext ctx) {
        ST st = m_group.getInstanceOf("TypeFloatingPointNumber");
        int precision = Defaults.FLOAT_PRECISION;

        if (ctx.IntegerConstant() != null) {
            precision = Integer.parseInt(ctx.IntegerConstant().getText());

            if (precision != Defaults.FLOAT_SHORT_PRECISION && precision != Defaults.FLOAT_LONG_PRECISION) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        st.add("precision", precision);

        return st;
    }

    @Override
    public ST visitProblem_part(SmallPearlParser.Problem_partContext ctx) {
        ST problem_part = m_group.getInstanceOf("ProblemPart");

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ScalarVariableDeclarationContext) {
                    problem_part.add("ScalarVariableDeclarations", visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.SemaDeclarationContext) {
                    problem_part.add("SemaDeclarations", visitSemaDeclaration((SmallPearlParser.SemaDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.BoltDeclarationContext) {
                    problem_part.add("BoltDeclarations", visitBoltDeclaration((SmallPearlParser.BoltDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.TaskDeclarationContext) {
                    problem_part.add("TaskDeclarations", visitTaskDeclaration((SmallPearlParser.TaskDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.DationSpecificationContext) {
                    problem_part.add("DationSpecifications", visitDationSpecification((SmallPearlParser.DationSpecificationContext) c));
                } else if (c instanceof SmallPearlParser.DationDeclarationContext) {
                    problem_part.add("DationDeclarations", visitDationDeclaration((SmallPearlParser.DationDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.ProcedureDeclarationContext) {
                    problem_part.add("ProcedureDeclarations", visitProcedureDeclaration((SmallPearlParser.ProcedureDeclarationContext) c));
                    problem_part.add("ProcedureSpecifications", getProcedureSpecification((SmallPearlParser.ProcedureDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.InterruptSpecificationContext) {
                    problem_part.add("InterruptSpecifications", visitInterruptSpecification((SmallPearlParser.InterruptSpecificationContext) c));
                }
            }
        }

        ST semaphoreArrays = m_group.getInstanceOf("TemporarySemaphoreArrays");
        LinkedList<LinkedList<SemaphoreEntry>>  listOfSemaphoreDeclarations = m_symbolTableVisitor.getListOfTemporarySemaphoreArrays();

        for (int i = 0; i < listOfSemaphoreDeclarations.size(); i++) {
            ST semaphoreArray = m_group.getInstanceOf("TemporarySemaphoreArray");
            LinkedList<SemaphoreEntry> listOfSemaphores = listOfSemaphoreDeclarations.get(i);
            for (int j = 0; j < listOfSemaphores.size(); j++) {
                semaphoreArray.add("semaphore", listOfSemaphores.get(j).getName());
            }
            semaphoreArrays.add("array", semaphoreArray);
        }

        problem_part.add("temporarySemaphoreArrays", semaphoreArrays);

        ST boltArrays = m_group.getInstanceOf("TemporaryBoltArrays");
        LinkedList<LinkedList<BoltEntry>>  listOfBoltDeclarations = m_symbolTableVisitor.getListOfTemporaryBoltArrays();

        for (int i = 0; i < listOfBoltDeclarations.size(); i++) {
            ST boltArray = m_group.getInstanceOf("TemporaryBoltArray");
            LinkedList<BoltEntry> listOfBolts = listOfBoltDeclarations.get(i);
            for (int j = 0; j < listOfBolts.size(); j++) {
                boltArray.add("bolt", listOfBolts.get(j).getName());
            }
            boltArrays.add("array", boltArray);
        }

        problem_part.add("temporaryBoltArrays", boltArrays);

        ST arrayDescriptors = m_group.getInstanceOf("ArrayDescriptors");
        LinkedList<ArrayDescriptor>  listOfArrayDescriptors = m_symbolTableVisitor.getListOfArrayDescriptors();

        for (int i = 0; i < listOfArrayDescriptors.size(); i++) {
            ST stArrayDescriptor = m_group.getInstanceOf("ArrayDescriptor");

            ArrayDescriptor arrayDescriptor = listOfArrayDescriptors.get(i);

            stArrayDescriptor.add("name",arrayDescriptor.getName());
            arrayDescriptors.add("descriptors", stArrayDescriptor);

            ArrayList<ArrayDimension> listOfArrayDimensions = arrayDescriptor.getDimensions();

            ST stArrayLimits = m_group.getInstanceOf("ArrayLimits");

            for(int j = 0; j < listOfArrayDimensions.size(); j++) {
                ST stArrayLimit = m_group.getInstanceOf("ArrayLimit");
                int lo = listOfArrayDimensions.get(j).getLowerBoundary();
                int up = listOfArrayDimensions.get(j).getUpperBoundary();
                stArrayLimit.add("lowerBoundary",Integer.toString(listOfArrayDimensions.get(j).getLowerBoundary()));
                stArrayLimit.add("upperBoundary",Integer.toString(listOfArrayDimensions.get(j).getUpperBoundary()));

                int noOfElemenstOnNextSubArray = 0;
                for(int k = j +1; k < listOfArrayDimensions.size(); k++) {
                    noOfElemenstOnNextSubArray += listOfArrayDimensions.get(k).getNoOfElements();
                }

                if ( noOfElemenstOnNextSubArray == 0) {
                    noOfElemenstOnNextSubArray = 1;
                }

                stArrayLimit.add("noOfElemenstOnNextSubArray", noOfElemenstOnNextSubArray);
                stArrayLimits.add("limits", stArrayLimit);
            }

            stArrayDescriptor.add("limits",stArrayLimits);
            stArrayDescriptor.add("dimensions",Integer.toString(listOfArrayDimensions.size()));
        }

        problem_part.add("ArrayDescriptors", arrayDescriptors);


        return problem_part;
    }

    @Override
    public ST visitSemaDeclaration(SmallPearlParser.SemaDeclarationContext ctx) {
        ST st = m_group.getInstanceOf("SemaDeclaration");
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

            if (ctx.preset() != null) {
                presetList = getPreset(ctx.preset());

                if (identifierDenotationList.size() != presetList.size()) {
                    throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
        }

        for (int i = 0; i < identifierDenotationList.size(); i++) {
            ST v = m_group.getInstanceOf("sema_declaration");
            v.add("name", identifierDenotationList.get(i));
            v.add("global", hasGlobalAttribute);

            if (presetList != null)
                v.add("preset", presetList.get(i));

            st.add("decl", v);
        }

        return st;
    }

    @Override
    public ST visitBoltDeclaration(SmallPearlParser.BoltDeclarationContext ctx) {
        ST st = m_group.getInstanceOf("BoltDeclaration");
        boolean hasGlobalAttribute = false;

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
            ST v = m_group.getInstanceOf("bolt_declaration");
            v.add("name", identifierDenotationList.get(i));
            v.add("global", hasGlobalAttribute);

            st.add("decl", v);
        }

        return st;
    }

    @Override
    public ST visitBoltReserve(SmallPearlParser.BoltReserveContext ctx) {
        ST st = m_group.getInstanceOf("BoltReserve");
        LinkedList<String> listOfNames = new LinkedList<String>();

        for (int i = 0; i < ctx.ID().size(); i++) {
            listOfNames.add(ctx.ID(i).getText());
        }

        Collections.sort(listOfNames);

        for (int i = 0; i < listOfNames.size(); i++) {
            st.add("names", listOfNames.get(i));
        }

        st.add("noofbolts", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitBoltFree(SmallPearlParser.BoltFreeContext ctx) {
        ST st = m_group.getInstanceOf("BoltFree");
        LinkedList<String> listOfNames = new LinkedList<String>();

        for (int i = 0; i < ctx.ID().size(); i++) {
            listOfNames.add(ctx.ID(i).getText());
        }

        Collections.sort(listOfNames);

        for (int i = 0; i < listOfNames.size(); i++) {
            st.add("names", listOfNames.get(i));
        }

        st.add("noofbolts", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitBoltEnter(SmallPearlParser.BoltEnterContext ctx) {
        ST st = m_group.getInstanceOf("BoltEnter");
        LinkedList<String> listOfNames = new LinkedList<String>();

        for (int i = 0; i < ctx.ID().size(); i++) {
            listOfNames.add(ctx.ID(i).getText());
        }

        Collections.sort(listOfNames);

        for (int i = 0; i < listOfNames.size(); i++) {
            st.add("names", listOfNames.get(i));
        }

        st.add("noofbolts", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitBoltLeave(SmallPearlParser.BoltLeaveContext ctx) {
        ST st = m_group.getInstanceOf("BoltLeave");
        LinkedList<String> listOfNames = new LinkedList<String>();

        for (int i = 0; i < ctx.ID().size(); i++) {
            listOfNames.add(ctx.ID(i).getText());
        }

        Collections.sort(listOfNames);

        for (int i = 0; i < listOfNames.size(); i++) {
            st.add("names", listOfNames.get(i));
        }

        st.add("noofbolts", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        ST taskdecl = m_group.getInstanceOf("task_declaration");
        ST priority = m_group.getInstanceOf("expression");
        Integer main = 0;

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);

        if (ctx.priority() != null) {
            priority = getExpression(ctx.priority().expression());
        }
        else {
            priority.add("code", Defaults.DEFAULT_TASK_PRIORITY);
        }

        if (ctx.task_main() != null) {
            main = 1;
        }

        taskdecl.add("name", ctx.ID());
        taskdecl.add("priority", priority);
        taskdecl.add("main", main);

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.TaskBodyContext) {
                    taskdecl.add("body", visitTaskBody((SmallPearlParser.TaskBodyContext) c));
                } else if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                    taskdecl.add("cpp", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                }
            }
        }

        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return taskdecl;
    }

    @Override
    public ST visitTaskBody(SmallPearlParser.TaskBodyContext ctx) {
        ST taskbody = m_group.getInstanceOf("task_body");

        if (ctx != null && ctx.children != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ScalarVariableDeclarationContext) {
                    taskbody.add("decls", visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.ArrayVariableDeclarationContext) {
                    taskbody.add("decls", visitArrayVariableDeclaration((SmallPearlParser.ArrayVariableDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.StatementContext) {
                    taskbody.add("statements", visitStatement((SmallPearlParser.StatementContext) c));
                } else if (c instanceof SmallPearlParser.DationDeclarationContext) {
                    taskbody.add("decls", visitDationDeclaration((SmallPearlParser.DationDeclarationContext) c));
                }
            }
        }

        return taskbody;
    }

    @Override
    public ST visitTimeConstant(SmallPearlParser.TimeConstantContext ctx) {
        ST st = m_group.getInstanceOf("TimeConstant");
        st.add("value", getTime(ctx));
        return st;
    }

    private ST getExpression(SmallPearlParser.ExpressionContext ctx) {
        ST st = m_group.getInstanceOf("expression");

        if (ctx != null ) {
            if (ctx instanceof SmallPearlParser.BaseExpressionContext) {
                st.add("code", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.AdditiveExpressionContext) {
                st.add("code", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.SubtractiveExpressionContext) {
                st.add("code", visitSubtractiveExpression(((SmallPearlParser.SubtractiveExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.MultiplicativeExpressionContext) {
                st.add("code", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.DivideExpressionContext) {
                st.add("code", visitDivideExpression((SmallPearlParser.DivideExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.DivideIntegerExpressionContext) {
                st.add("code", visitDivideIntegerExpression((SmallPearlParser.DivideIntegerExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
                st.add("code", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
                st.add("code", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.ExponentiationExpressionContext) {
                st.add("code", visitExponentiationExpression((SmallPearlParser.ExponentiationExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.LtRelationalExpressionContext) {
                st.add("code", visitLtRelationalExpression((SmallPearlParser.LtRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.GeRelationalExpressionContext) {
                st.add("code", visitGeRelationalExpression((SmallPearlParser.GeRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.NeRelationalExpressionContext) {
                st.add("code", visitNeRelationalExpression((SmallPearlParser.NeRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.EqRelationalExpressionContext) {
                st.add("code", visitEqRelationalExpression((SmallPearlParser.EqRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.GtRelationalExpressionContext) {
                st.add("code", visitGtRelationalExpression((SmallPearlParser.GtRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.LeRelationalExpressionContext) {
                st.add("code", visitLeRelationalExpression((SmallPearlParser.LeRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.AtanExpressionContext) {
                st.add("code", visitAtanExpression((SmallPearlParser.AtanExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.CosExpressionContext) {
                st.add("code", visitCosExpression((SmallPearlParser.CosExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.ExpExpressionContext) {
                st.add("code", visitExpExpression((SmallPearlParser.ExpExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.LnExpressionContext) {
                st.add("code", visitLnExpression((SmallPearlParser.LnExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SinExpressionContext) {
                st.add("code", visitSinExpression((SmallPearlParser.SinExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SqrtExpressionContext) {
                st.add("code", visitSqrtExpression((SmallPearlParser.SqrtExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.TanExpressionContext) {
                st.add("code", visitTanExpression((SmallPearlParser.TanExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.TanhExpressionContext) {
                st.add("code", visitTanhExpression((SmallPearlParser.TanhExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.FitExpressionContext) {
                st.add("code", visitFitExpression((SmallPearlParser.FitExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.ExponentiationExpressionContext) {
                st.add("code", visitExponentiationExpression((SmallPearlParser.ExponentiationExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.AbsExpressionContext) {
                st.add("code", visitAbsExpression((SmallPearlParser.AbsExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SizeofExpressionContext) {
                st.add("code", visitSizeofExpression((SmallPearlParser.SizeofExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.EntierExpressionContext) {
                st.add("code", visitEntierExpression((SmallPearlParser.EntierExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.RoundExpressionContext) {
                st.add("code", visitRoundExpression((SmallPearlParser.RoundExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SignExpressionContext) {
                st.add("code", visitSignExpression((SmallPearlParser.SignExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.RemainderExpressionContext) {
                st.add("code", visitRemainderExpression((SmallPearlParser.RemainderExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.NowFunctionContext) {
                st.add("code", visitNowFunction((SmallPearlParser.NowFunctionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.AndExpressionContext) {
                st.add("code", visitAndExpression(((SmallPearlParser.AndExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.OrExpressionContext) {
                st.add("code", visitOrExpression(((SmallPearlParser.OrExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.ExorExpressionContext) {
                st.add("code", visitExorExpression(((SmallPearlParser.ExorExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.CshiftExpressionContext) {
                st.add("code", visitCshiftExpression(((SmallPearlParser.CshiftExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.ShiftExpressionContext) {
                st.add("code", visitShiftExpression(((SmallPearlParser.ShiftExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.CatExpressionContext) {
                st.add("code", visitCatExpression(((SmallPearlParser.CatExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.NotExpressionContext) {
                st.add("code", visitNotExpression(((SmallPearlParser.NotExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.TOFIXEDExpressionContext) {
                st.add("code", visitTOFIXEDExpression(((SmallPearlParser.TOFIXEDExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.TOFLOATExpressionContext) {
                st.add("code", visitTOFLOATExpression(((SmallPearlParser.TOFLOATExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.TOBITExpressionContext) {
                st.add("code", visitTOBITExpression(((SmallPearlParser.TOBITExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.CONTExpressionContext) {
                st.add("code", visitCONTExpression(((SmallPearlParser.CONTExpressionContext) ctx)));
            }
        }
        else {
            st = null;
        }

        return st;
    }

    private ST getReferenceExpression(SmallPearlParser.ExpressionContext ctx) {
        ST st = m_group.getInstanceOf("ReferenceExpression");

        if (ctx != null ) {
            if (ctx instanceof SmallPearlParser.BaseExpressionContext) {
                st.add("code", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.AdditiveExpressionContext) {
                st.add("code", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.SubtractiveExpressionContext) {
                st.add("code", visitSubtractiveExpression(((SmallPearlParser.SubtractiveExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.MultiplicativeExpressionContext) {
                st.add("code", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.DivideExpressionContext) {
                st.add("code", visitDivideExpression((SmallPearlParser.DivideExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.DivideIntegerExpressionContext) {
                st.add("code", visitDivideIntegerExpression((SmallPearlParser.DivideIntegerExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
                st.add("code", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
                st.add("code", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.ExponentiationExpressionContext) {
                st.add("code", visitExponentiationExpression((SmallPearlParser.ExponentiationExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.LtRelationalExpressionContext) {
                st.add("code", visitLtRelationalExpression((SmallPearlParser.LtRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.GeRelationalExpressionContext) {
                st.add("code", visitGeRelationalExpression((SmallPearlParser.GeRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.NeRelationalExpressionContext) {
                st.add("code", visitNeRelationalExpression((SmallPearlParser.NeRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.EqRelationalExpressionContext) {
                st.add("code", visitEqRelationalExpression((SmallPearlParser.EqRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.GtRelationalExpressionContext) {
                st.add("code", visitGtRelationalExpression((SmallPearlParser.GtRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.LeRelationalExpressionContext) {
                st.add("code", visitLeRelationalExpression((SmallPearlParser.LeRelationalExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.AtanExpressionContext) {
                st.add("code", visitAtanExpression((SmallPearlParser.AtanExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.CosExpressionContext) {
                st.add("code", visitCosExpression((SmallPearlParser.CosExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.ExpExpressionContext) {
                st.add("code", visitExpExpression((SmallPearlParser.ExpExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.LnExpressionContext) {
                st.add("code", visitLnExpression((SmallPearlParser.LnExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SinExpressionContext) {
                st.add("code", visitSinExpression((SmallPearlParser.SinExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SqrtExpressionContext) {
                st.add("code", visitSqrtExpression((SmallPearlParser.SqrtExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.TanExpressionContext) {
                st.add("code", visitTanExpression((SmallPearlParser.TanExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.TanhExpressionContext) {
                st.add("code", visitTanhExpression((SmallPearlParser.TanhExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.FitExpressionContext) {
                st.add("code", visitFitExpression((SmallPearlParser.FitExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.ExponentiationExpressionContext) {
                st.add("code", visitExponentiationExpression((SmallPearlParser.ExponentiationExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.AbsExpressionContext) {
                st.add("code", visitAbsExpression((SmallPearlParser.AbsExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SizeofExpressionContext) {
                st.add("code", visitSizeofExpression((SmallPearlParser.SizeofExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.EntierExpressionContext) {
                st.add("code", visitEntierExpression((SmallPearlParser.EntierExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.RoundExpressionContext) {
                st.add("code", visitRoundExpression((SmallPearlParser.RoundExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.SignExpressionContext) {
                st.add("code", visitSignExpression((SmallPearlParser.SignExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.RemainderExpressionContext) {
                st.add("code", visitRemainderExpression((SmallPearlParser.RemainderExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.NowFunctionContext) {
                st.add("code", visitNowFunction((SmallPearlParser.NowFunctionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.AndExpressionContext) {
                st.add("code", visitAndExpression(((SmallPearlParser.AndExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.OrExpressionContext) {
                st.add("code", visitOrExpression(((SmallPearlParser.OrExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.ExorExpressionContext) {
                st.add("code", visitExorExpression(((SmallPearlParser.ExorExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.CshiftExpressionContext) {
                st.add("code", visitCshiftExpression(((SmallPearlParser.CshiftExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.ShiftExpressionContext) {
                st.add("code", visitShiftExpression(((SmallPearlParser.ShiftExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.CatExpressionContext) {
                st.add("code", visitCatExpression(((SmallPearlParser.CatExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.NotExpressionContext) {
                st.add("code", visitNotExpression(((SmallPearlParser.NotExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.TOFIXEDExpressionContext) {
                st.add("code", visitTOFIXEDExpression(((SmallPearlParser.TOFIXEDExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.TOFLOATExpressionContext) {
                st.add("code", visitTOFLOATExpression(((SmallPearlParser.TOFLOATExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.TOBITExpressionContext) {
                st.add("code", visitTOBITExpression(((SmallPearlParser.TOBITExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.TaskFunctionContext) {
                st.add("code", visitTaskFunction(((SmallPearlParser.TaskFunctionContext) ctx)));
            }

        }

        return st;
    }

    @Override
    public ST visitStartConditionAT(SmallPearlParser.StartConditionATContext ctx) {
        ST st = m_group.getInstanceOf("StartConditionAT");

/* TODO
        if (ctx.expression() instanceof SmallPearlParser.BaseExpressionContext) {
            st.add("rhs", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.AdditiveExpressionContext) {
            st.add("rhs", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.MultiplicativeExpressionContext) {
            st.add("rhs", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
            st.add("rhs", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
            st.add("rhs", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx.expression()));

//        } else if (ctx.expression() instanceof SmallPearlParser.RelationalExpressionContext) {
//            st.add("rhs", visitRelationalExpression((SmallPearlParser.RelationalExpressionContext) ctx.expression()));
        }
*/
        return st;
    }

    @Override
    public ST visitStartConditionAFTER(SmallPearlParser.StartConditionAFTERContext ctx) {
        ST st = m_group.getInstanceOf("StartConditionAFTER");
/* TODO
        if (ctx.expression() instanceof SmallPearlParser.BaseExpressionContext) {
            st.add("rhs", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.AdditiveExpressionContext) {
            st.add("rhs", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.MultiplicativeExpressionContext) {
            st.add("rhs", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
            st.add("rhs", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
            st.add("rhs", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx.expression()));
        }
*/
        return st;
    }

    @Override
    public ST visitStatement(SmallPearlParser.StatementContext ctx) {
        ST stmt = m_group.getInstanceOf("statement");

        stmt.add("srcFilename", m_sourceFileName);
        stmt.add("srcLine", ctx.start.getLine());
        stmt.add("srcColumn", ctx.start.getCharPositionInLine());

        if (ctx != null) {
            if ( ctx.label_statement() != null ) {
                for (int i = 0; i < ctx.label_statement().size(); i++) {
                    stmt.add("label", visitLabel_statement(ctx.label_statement(i)));
                }
            }

            if (ctx.children != null) {
                for (ParseTree c : ctx.children) {
                    if (c instanceof SmallPearlParser.Unlabeled_statementContext) {
                        stmt.add("code", visitUnlabeled_statement((SmallPearlParser.Unlabeled_statementContext) c));
                    } else if (c instanceof SmallPearlParser.Block_statementContext) {
                        stmt.add("code", visitBlock_statement((SmallPearlParser.Block_statementContext) c));
                    } else if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                        stmt.add("cpp", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                    }
                }
            }
        }

        return stmt;
    }

    @Override
    public ST visitLabel_statement(SmallPearlParser.Label_statementContext ctx) {
        ST st = m_group.getInstanceOf("label_statement");
        st.add( "label", ctx.ID().getText());
        return st;
    }

    @Override
    public ST visitGotoStatement(SmallPearlParser.GotoStatementContext ctx) {
        ST st = m_group.getInstanceOf("goto_statement");
        st.add( "label", ctx.ID().getText());
        return st;
    }

    @Override
    public ST visitUnlabeled_statement(SmallPearlParser.Unlabeled_statementContext ctx) {
        ST statement = m_group.getInstanceOf("statement");

        if (ctx.empty_statement() != null) {
            statement.add("code", visitEmpty_statement(ctx.empty_statement()));
        } else if (ctx.assignment_statement() != null) {
            statement.add("code", visitAssignment_statement(ctx.assignment_statement()));
        } else if (ctx.sequential_control_statement() != null) {
            statement.add("code", visitSequential_control_statement(ctx.sequential_control_statement()));
        } else if (ctx.realtime_statement() != null) {
            statement.add("code", visitRealtime_statement(ctx.realtime_statement()));
        } else if (ctx.interrupt_statement() != null) {
            statement.add("code", visitInterrupt_statement(ctx.interrupt_statement()));
        } else if (ctx.io_statement() != null) {
            statement.add("code", visitIo_statement(ctx.io_statement()));
        } else if (ctx.callStatement() != null) {
            statement.add("code", visitCallStatement(ctx.callStatement()));
        } else if (ctx.returnStatement() != null) {
            statement.add("code", visitReturnStatement(ctx.returnStatement()));
        } else if (ctx.loopStatement() != null) {
            statement.add("code", visitLoopStatement(ctx.loopStatement()));
        } else if (ctx.exitStatement() != null) {
            statement.add("code", visitExitStatement(ctx.exitStatement()));
        } else if (ctx.gotoStatement() != null) {
            statement.add("code", visitGotoStatement(ctx.gotoStatement()));
        } else if (ctx.convertStatement() != null) {
            statement.add("code", visitConvertStatement(ctx.convertStatement()));
        }


        return statement;
    }

    @Override
    public ST visitSequential_control_statement(SmallPearlParser.Sequential_control_statementContext ctx) {
        ST statement = m_group.getInstanceOf("statement");

        if (ctx.if_statement() != null) {
            statement.add("code", visitIf_statement(ctx.if_statement()));
        }
        else if ( ctx.case_statement() != null) {
            statement.add("code", visitCase_statement(ctx.case_statement()));
        }

        return statement;
    }

    @Override
    public ST visitIf_statement(SmallPearlParser.If_statementContext ctx) {
        ST stmt = m_group.getInstanceOf("if_statement");

        TypeDefinition typeDef = m_expressionTypeVisitor.lookupType(ctx.expression());

//        if ( typeDef instanceof TypeBit ) {
//            TypeBit typeBit = (TypeBit) typeDef;
//
//            if ( typeBit.getPrecision() == 1 ) {
//                stmt.add("rhs",  getExpression(ctx.expression()));
//            }
//            else {
//                ST cast = m_group.getInstanceOf("CastBitToBoolean");
//                cast.add("name", getExpression(ctx.expression()));
//                stmt.add("rhs", cast);
//            }
//        }
//        else {
            ST cast = m_group.getInstanceOf("CastBitToBoolean");
            cast.add("name", getExpression(ctx.expression()));
            stmt.add("rhs", cast);
//        }

        if (ctx.then_block() != null) {
            stmt.add("then_block", visitThen_block(ctx.then_block()));
        }

        if (ctx.else_block() != null) {
            stmt.add("else_block", visitElse_block(ctx.else_block()));
        }

        return stmt;
    }

    @Override
    public ST visitElse_block(SmallPearlParser.Else_blockContext ctx) {
        ST statement = m_group.getInstanceOf("statement");

        int i;
        for (i = 0; i < ctx.statement().size(); i++) {
            statement.add("code", visitStatement(ctx.statement(i)));
        }

        return statement;
    }

    @Override
    public ST visitThen_block(SmallPearlParser.Then_blockContext ctx) {
        ST statement = m_group.getInstanceOf("statement");

        int i;
        for (i = 0; i < ctx.statement().size(); i++) {
            statement.add("code", visitStatement(ctx.statement(i)));
        }

        return statement;
    }


    @Override
    public ST visitCase_statement(SmallPearlParser.Case_statementContext ctx) {
        ST st = m_group.getInstanceOf("CaseStatement");

        if ( ctx.case_statement_selection1() != null ) {
            st.add("casestatement1", visitCase_statement_selection1(ctx.case_statement_selection1()));
        }
        else if ( ctx.case_statement_selection2() != null ) {
            st.add("casestatement2", visitCase_statement_selection2(ctx.case_statement_selection2()));
        }

        return st;
    }

    @Override
    public ST visitCase_statement_selection1(SmallPearlParser.Case_statement_selection1Context ctx) {
        ST st = m_group.getInstanceOf("CaseStatement1");
        ST st_alt = m_group.getInstanceOf("CaseAlternatives");

        st.add("expression", getExpression(ctx.expression()));

        for( int i = 0; i < ctx.case_statement_selection1_alt().size(); i++) {
            SmallPearlParser.Case_statement_selection1_altContext alt = ctx.case_statement_selection1_alt(i);

            ST cur_alt = visitCase_statement_selection1_alt(alt);
            cur_alt.add("alt", i+1);
            st_alt.add( "Alternatives", cur_alt);
        }

        st.add("alternatives", st_alt);

        if ( ctx.case_statement_selection_out() != null ) {
            st.add("out", visitCase_statement_selection_out(ctx.case_statement_selection_out()));
        }

        return st;
    }

    @Override
    public ST visitCase_statement_selection1_alt(SmallPearlParser.Case_statement_selection1_altContext ctx) {
        ST st = m_group.getInstanceOf("CaseAlternative");

        for ( int i = 0; i < ctx.statement().size(); i++) {
            st.add("statements", visitStatement(ctx.statement(i)));
        }

        return st;
    }

    @Override
    public ST visitCase_statement_selection_out(SmallPearlParser.Case_statement_selection_outContext ctx) {
        ST st = m_group.getInstanceOf("CaseOut");

        for ( int i = 0; i < ctx.statement().size(); i++) {
            st.add("statements", visitStatement(ctx.statement(i)));
        }

        return st;
    }

    @Override
    public ST visitCase_statement_selection2(SmallPearlParser.Case_statement_selection2Context ctx) {
        ST st = m_group.getInstanceOf("CaseStatement2");
        ST st_alt = m_group.getInstanceOf("CaseAlternatives");

        st.add("expression", getExpression(ctx.expression()));

        for( int i = 0; i < ctx.case_statement_selection2_alt().size(); i++) {
            SmallPearlParser.Case_statement_selection2_altContext alt = ctx.case_statement_selection2_alt(i);
            ST cur_alt = visitCase_statement_selection2_alt(alt);
            st_alt.add( "Alternatives", cur_alt);
        }

        st.add("alternatives", st_alt);

        if ( ctx.case_statement_selection_out() != null ) {
            st.add("out", visitCase_statement_selection_out(ctx.case_statement_selection_out()));
        }

        return st;
    }

    @Override
    public ST visitCase_statement_selection2_alt(SmallPearlParser.Case_statement_selection2_altContext ctx) {
        ST st = m_group.getInstanceOf("CaseAlternative2");

        st.add("alts", visitCase_list(ctx.case_list()));

        for ( int i = 0; i < ctx.statement().size(); i++) {
            st.add("statements", visitStatement(ctx.statement(i)));
        }

        return st;
    }

    @Override
    public ST visitCase_list(SmallPearlParser.Case_listContext ctx) {
        ST st = m_group.getInstanceOf("CaseIndexList");

        for (int i = 0; i < ctx.index_section().size(); i++){
            SmallPearlParser.Index_sectionContext index = ctx.index_section(i);

            if ( index.expression().size() == 1) {
                boolean old_map_to_const = m_map_to_const; // very ugly, but did not found proper solution yet :-(
                ST st_index = m_group.getInstanceOf("CaseIndex");

                m_map_to_const = false;
                ST expr = getExpression(index.expression(0));
                m_map_to_const = old_map_to_const;

                st_index.add("index", expr);
                st.add("indices", st_index);
            }
            else if ( index.expression().size() == 2) {
                boolean old_map_to_const = m_map_to_const; // very ugly, but did not found proper solution yet :-(

                ST st_range = m_group.getInstanceOf("CaseRange");

                m_map_to_const = false;
                st_range.add("from", getExpression(index.expression(0)));
                st_range.add("to", getExpression(index.expression(1)));
                m_map_to_const = old_map_to_const;

                st.add("indices", st_range);
            }
        }

        return st;
    }

    @Override
    public ST visitEmpty_statement(SmallPearlParser.Empty_statementContext ctx) {
        ST statement = m_group.getInstanceOf("empty_statement");
        return statement;
    }

    @Override
    public ST visitAssignment_statement(SmallPearlParser.Assignment_statementContext ctx) {
        ST stmt = null;
        String id = null;

        if ( ctx.ID() != null ) {
            id = ctx.ID().getText();
        } else if ( ctx.stringSelection() != null ) {
            if ( ctx.stringSelection().charSelection() != null ) {
                id = ctx.stringSelection().charSelection().ID().getText();
            }
            else if ( ctx.stringSelection().bitSelection() != null ) {
                id = ctx.stringSelection().bitSelection().ID().getText();
            }
        }

        SymbolTableEntry entry = m_currentSymbolTable.lookup(id);

        if ( entry == null ) {
            m_currentSymbolTable.dump();
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        TypeDefinition rhs_type = m_expressionTypeVisitor.lookupType(ctx.expression());

//        if ( variable.getType() instanceof TypeReference ) {
//            TypeReference lhs_type = (TypeReference) variable.getType();
//            TypeDefinition rhs_type;
//
//            if ( ctx.dereference() == null ) {
//                if ( rhs1.getVariable() == null ) {
//                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
//                }
//
//                TypeDefinition lt = lhs_type.getBaseType();
//
//                if ( rhs instanceof TypeReference) {
//                    rhs_type = ((TypeReference) rhs).getBaseType();
//                }
//                else {
//                    rhs_type = rhs;
//                }
//
//                if ( !(lt.equals(rhs_type))) {
//                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
//                }
//            }
//            else {
//                TypeDefinition lt = lhs_type.getBaseType();
//                if ( !(lt.equals(rhs))) {
//                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
//                }
//            }
//        }

        if (entry instanceof VariableEntry) {
            VariableEntry variable = (VariableEntry)entry;
            TypeDefinition lhs_type = variable.getType();

            if ( lhs_type instanceof TypeFixed) {
                TypeFixed typ = (TypeFixed)(lhs_type);
                m_currFixedLength = typ.getPrecision();
            }
            else {
                m_currFixedLength  = null;
            }


            if ( lhs_type instanceof TypeReference) {
                ST st = m_group.getInstanceOf("assignment_statement");

                if ( ctx.dereference() != null ) {
                    ST dereference = m_group.getInstanceOf("CONT");
                    dereference.add("operand", getUserVariable(ctx.ID().getText()));
                    st.add("lhs", dereference);
                    st.add("rhs", getExpression(ctx.expression()));
                    stmt = st;
                }
                else {
                    st.add("lhs", getUserVariable(ctx.ID().getText()));

                    if ( rhs_type instanceof TypeReference) {
                        st.add("rhs", getExpression(ctx.expression()));
                    }
                    else if ( rhs_type instanceof TypeTask) {
                        ST stTask = m_group.getInstanceOf("TASK");
                        st.add("rhs", stTask);
                    } else {
                        st.add("rhs", getReferenceExpression(ctx.expression()));
                    }
                    stmt=st;
                }
            }
            else {
                if ( lhs_type instanceof TypeArray) {
                    ST st = m_group.getInstanceOf("assignment_statement");
                    ST array = m_group.getInstanceOf("ArrayLHS");

                    ArrayDescriptor array_descriptor = new ArrayDescriptor(((TypeArray)lhs_type).getNoOfDimensions(),((TypeArray)lhs_type).getDimensions());

                    array.add("name", variable.getName());
                    array.add("descriptor", array_descriptor.getName());

                    ST indices = m_group.getInstanceOf("ArrayIndices");

                    indices.add("indices",visitIndices(ctx.indices()));
                    array.add("indices", indices);
                    st.add("lhs", array);
                    st.add("rhs", getExpression(ctx.expression()));
                    stmt = st;
                } else {
                    if ( ctx.stringSelection() != null ) {
                        if (ctx.stringSelection().charSelection() != null) {
                            ST st = m_group.getInstanceOf("assignmentStatementCharSlice");

                            st.add("id", getUserVariable(id));

                            SmallPearlParser.CharSelectionContext c = (SmallPearlParser.CharSelectionContext) ctx.stringSelection().charSelection();
                            for (int i = 0; i < c.charSelectionSlice().size(); i++) {
                                ST slice= m_group.getInstanceOf("CharSlice");

                                slice.add("lwb", getExpression(c.charSelectionSlice(i).expression(0)));

                                if ( c.charSelectionSlice(i).expression().size() == 2 ) {
                                    slice.add("upb", getExpression(c.charSelectionSlice(i).expression(1)));
                                } else {
                                    slice.add("upb", getExpression(c.charSelectionSlice(i).expression(0)));
                                }

                                st.add("lhs", slice);

                            }

                            ST setCharSlice = m_group.getInstanceOf("SetCharSlice");

                            setCharSlice.add("expr", getExpression(ctx.expression()));
                            st.add("lhs",setCharSlice);
                            stmt = st;
                        }
                        else  if (ctx.stringSelection().bitSelection() != null) {
                            ST st = m_group.getInstanceOf("assignmentStatementBitSlice");

                            st.add("id", getUserVariable(id));

                            SmallPearlParser.BitSelectionContext c = (SmallPearlParser.BitSelectionContext) ctx.stringSelection().bitSelection();
                            for (int i = 0; i < c.bitSelectionSlice().size(); i++) {
                                ST slice = m_group.getInstanceOf("GetBitSlice");

                                slice.add("lwb", getExpression(c.bitSelectionSlice(i).expression(0)));

                                if ( c.bitSelectionSlice(i).expression().size() == 2 ) {
                                    slice.add("upb", getExpression(c.bitSelectionSlice(i).expression(1)));
                                } else {
                                    slice.add("upb", getExpression(c.bitSelectionSlice(i).expression(0)));
                                }

                                st.add("slices", slice);
                            }

                            ST setBitSlice = m_group.getInstanceOf("SetBitSlice");

                            setBitSlice.add("expr", getExpression(ctx.expression()));
                            setBitSlice.add("size", 42);

                            st.add("rhs", setBitSlice);

                            stmt = st;
                        }
                        else {
                            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                        }
                    } else {
                        ST st = m_group.getInstanceOf("assignment_statement");
                        st.add("lhs", getUserVariable(id));
                        st.add("rhs", getExpression(ctx.expression()));
                        stmt = st;
                    }
                }
            }
        }
        else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        m_currFixedLength = null;

        return stmt;
    }


    @Override
    public ST visitIndices(SmallPearlParser.IndicesContext ctx ) {
        ST indices = m_group.getInstanceOf("ArrayIndices");


        for ( int i = 0; i < ctx.expression().size(); i++) {
            ST stIndex = m_group.getInstanceOf("ArrayIndex");
            stIndex.add("index", getExpression(ctx.expression(i)));
            indices.add("indices", stIndex);
        }

        return indices;
    }

    @Override
    public ST visitRealtime_statement(SmallPearlParser.Realtime_statementContext ctx) {
        ST statement = m_group.getInstanceOf("statement");

        if (ctx.task_control_statement() != null) {
            statement.add("code", visitTask_control_statement(ctx.task_control_statement()));
        } else if (ctx.task_coordination_statement() != null) {
            statement.add("code", visitTask_coordination_statement(ctx.task_coordination_statement()));
        }

        return statement;

    }

    @Override
    public ST visitBaseExpression(SmallPearlParser.BaseExpressionContext ctx) {
        ST expression = m_group.getInstanceOf("expression");

        if (ctx.primaryExpression() != null) {
            expression.add("code", visitPrimaryExpression(ctx.primaryExpression()));
        }

        return expression;

    }

    private String getBitStringLiteral(String literal) {
        return CommonUtils.convertBitStringToLong(literal).toString();
    }

    private int getBitStringLength(String literal) {
        return  CommonUtils.getBitStringLength(literal);
    }

    @Override
    public ST visitPrimaryExpression(SmallPearlParser.PrimaryExpressionContext ctx) {
        ST expression = m_group.getInstanceOf("expression");

        if (ctx.literal() != null) {
//            if (ctx.literal().BitStringLiteral() != null) {
//                ST bitstring = m_group.getInstanceOf("BitStringConstant");
//                // TODO:
//                // ! assignment
//                // b1 := '0'B1;
//                // !__cpp__('_b1 = pearlrt::BitString<1>(0);			');
//                // b4i2 := '8'B4;
//                // __cpp__('_b4i2= pearlrt::BitString<4>(8);			');
//                bitstring.add("value", getBitStringLiteral(ctx.literal().BitStringLiteral().getText()));
//                bitstring.add("length", getBitStringLength(ctx.literal().BitStringLiteral().getText()));
//
//                expression.add("bitstring", bitstring);
//            } else {
                expression.add("code", visitLiteral(ctx.literal()));
//            }
        } else if (ctx.ID() != null) {
            SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.ID().getText());

            if ( entry instanceof org.smallpearl.compiler.SymbolTable.ProcedureEntry ) {
                ST functionCall = m_group.getInstanceOf("FunctionCall");
                functionCall.add("callee", ctx.ID().getText());

                if ( ctx.expression() != null ){
                    functionCall.add("ListOfActualParameters", getActualParameters(ctx.expression()));
                }

                expression.add("functionCall", functionCall);
            }
            else if ( entry instanceof org.smallpearl.compiler.SymbolTable.VariableEntry ) {
                org.smallpearl.compiler.SymbolTable.VariableEntry variable = (org.smallpearl.compiler.SymbolTable.VariableEntry)entry;

                if ( variable.getType() instanceof TypeBit ) {
                    TypeBit type = (TypeBit) variable.getType();
                    expression.add("id", getUserVariable(ctx.ID().getText()));
                }
                else if ( variable.getType() instanceof TypeArray ) {
                    ST array = m_group.getInstanceOf("ArrayLHS");

                    TypeArray type = (TypeArray) variable.getType();
                    ArrayDescriptor array_descriptor = new ArrayDescriptor(type.getNoOfDimensions(),type.getDimensions());
                    array.add("name", variable.getName());
                    array.add("descriptor", array_descriptor.getName());
                    array.add("indices", getIndices(ctx.expression()));

                    expression.add("id", array);
                }
                else {
                    expression.add("id", getUserVariable(ctx.ID().getText()));
                }
            }
            else {
                expression.add("id", getUserVariable(ctx.ID().getText()));
            }
        } else if (ctx.semaTry() != null) {
            expression.add("code", visitSemaTry(ctx.semaTry()));
        } else if (ctx.stringSlice()!= null) {
            expression.add("code", visitStringSlice(ctx.stringSlice()));
        } else if (ctx.expression() != null) {
            expression.add("code", "(");
            expression.add("code", visit(ctx.expression(0)));
            expression.add("code", ")");
        } else if (ctx.stringSlice() != null) {
            expression.add("code", visitStringSlice(ctx.stringSlice()));
        }

        return expression;
    }


    @Override
    public ST visitStringSlice(SmallPearlParser.StringSliceContext ctx) {
        ST st = m_group.getInstanceOf("StringSlice");

        if (m_debug) {
            System.out.println("CppCodeGeneratorVisitor: visitStringSlice");
        }

        if ( ctx.charSlice() instanceof SmallPearlParser.Case1CharSliceContext) {
           st = visitCase1CharSlice((SmallPearlParser.Case1CharSliceContext)(ctx.charSlice()));
        }
        else if ( ctx.charSlice() instanceof SmallPearlParser.Case2CharSliceContext) {
            st = visitCase2CharSlice((SmallPearlParser.Case2CharSliceContext)(ctx.charSlice()));
        }
        else  if ( ctx.charSlice() instanceof SmallPearlParser.Case3CharSliceContext) {
            st = visitCase3CharSlice((SmallPearlParser.Case3CharSliceContext)(ctx.charSlice()));
        }
        else if ( ctx.charSlice() instanceof SmallPearlParser.Case4CharSliceContext) {
            st = visitCase4CharSlice((SmallPearlParser.Case4CharSliceContext)(ctx.charSlice()));
        }
        else if ( ctx.bitSlice() instanceof SmallPearlParser.Case1BitSliceContext) {
            st = visitCase1BitSlice((SmallPearlParser.Case1BitSliceContext)(ctx.bitSlice()));
        }
        else if ( ctx.bitSlice() instanceof SmallPearlParser.Case2BitSliceContext) {
            st = visitCase2BitSlice((SmallPearlParser.Case2BitSliceContext)(ctx.bitSlice()));
        }
        else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }


//        if ( ctx.charSlice() != null ) {
//            st.add("id", ctx.charSlice().ID().getText());
//            st.add("lwb", getExpression(ctx.charSlice().expression(0)));
//
//            System.out.println(">>> EXPR="+ctx.charSlice().expression(0).getText());
//
//            if ( ctx.charSlice().expression().size() == 1) {
//                st.add("upb", getExpression(ctx.charSlice().expression(0)));
//                size = "1";
//            }
//            else {
//                st.add("upb", getExpression(ctx.charSlice().expression(1)));
//                size = "( (" +
//                        getExpression(ctx.charSlice().expression(1)).render() +
//                        ") - (" +
//                        getExpression(ctx.charSlice().expression(0)).render() +
//                        ") + 1)";
//            }
//
//            st.add("size", size);
//        }
//        else if ( ctx.bitSlice() != null ) {
//
//        }
//        else {
//            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
//        }

        return st;
    }

    @Override
    public ST visitAdditiveExpression(SmallPearlParser.AdditiveExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("expression");

        expr.add("code", visit(ctx.expression(0)));
        expr.add("code", ctx.op.getText());
        expr.add("code", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitSubtractiveExpression(SmallPearlParser.SubtractiveExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("expression");

        expr.add("code", visit(ctx.expression(0)));
        expr.add("code", ctx.op.getText());
        expr.add("code", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitMultiplicativeExpression(SmallPearlParser.MultiplicativeExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("expression");

        expr.add("code", visit(ctx.expression(0)));
        expr.add("code", ctx.op.getText());
        expr.add("code", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitDivideExpression(SmallPearlParser.DivideExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("expression");

        expr.add("code", visit(ctx.expression(0)));
        expr.add("code", ctx.op.getText());
        expr.add("code", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitDivideIntegerExpression(SmallPearlParser.DivideIntegerExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("expression");

        expr.add("code", visit(ctx.expression(0)));
        expr.add("code", "/");
        expr.add("code", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitUnaryExpression(SmallPearlParser.UnaryExpressionContext ctx) {
        ST st = m_group.getInstanceOf("expression");

//        expr.add( "code", visit(ctx.expression(0)));
//        expr.add( "code", ctx.op.getText());
//        expr.add( "code", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitUnaryAdditiveExpression(SmallPearlParser.UnaryAdditiveExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("expression");
        expr.add("code", visit(ctx.expression()));
        return expr;
    }

    @Override
    public ST visitUnarySubtractiveExpression(SmallPearlParser.UnarySubtractiveExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("expression");

        if (ctx.getChild(1) instanceof SmallPearlParser.BaseExpressionContext) {
            SmallPearlParser.BaseExpressionContext base_ctx = (SmallPearlParser.BaseExpressionContext) (ctx.getChild(1));

            if (base_ctx.primaryExpression() != null) {
                SmallPearlParser.PrimaryExpressionContext primary_ctx = base_ctx.primaryExpression();

                if (primary_ctx.getChild(0) instanceof SmallPearlParser.LiteralContext) {
                    SmallPearlParser.LiteralContext literal_ctx = (SmallPearlParser.LiteralContext) (primary_ctx.getChild(0));

                    if (literal_ctx.floatingPointConstant() != null) {
                        try {
                            double value = -1 * Double.parseDouble(literal_ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
                            int precision = m_currentSymbolTable.lookupDefaultFloatLength();
                            ConstantFloatValue float_value = new ConstantFloatValue(value,precision);
                            expr.add("code", float_value);
                        } catch (NumberFormatException ex) {
                            throw new NumberOutOfRangeException(ctx.getText(), literal_ctx.start.getLine(), literal_ctx.start.getCharPositionInLine());
                        }
                    }
                    else if (literal_ctx.fixedConstant() != null) {
                        try {
                            int value = 0;
                            int precision = m_currentSymbolTable.lookupDefaultFixedLength();

                            if ( literal_ctx.fixedConstant().fixedNumberPrecision() != null) {
                                precision = Integer.parseInt(literal_ctx.fixedConstant().fixedNumberPrecision().IntegerConstant().toString());
                            }

                            String s = literal_ctx.fixedConstant().IntegerConstant().toString();
                            value = -1 * Integer.parseInt(literal_ctx.fixedConstant().IntegerConstant().toString());

                            if (m_map_to_const) {
                                ConstantFixedValue fixed_value = new ConstantFixedValue(value, precision);
                                expr.add("code", fixed_value);
                            } else {
                                expr.add("code", value);
                            }
                        } catch (NumberFormatException ex) {
                            throw new NumberOutOfRangeException(ctx.getText(), literal_ctx.start.getLine(), literal_ctx.start.getCharPositionInLine());
                        }
                    }
                }
                else {
                    expr.add("code", ctx.op.getText());
                    expr.add("code", visit(ctx.expression()));
                }
            }
        }

        return expr;
    }

    @Override
    public ST visitExponentiationExpression(SmallPearlParser.ExponentiationExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("Exponentiation");

        expr.add("lhs", visit(ctx.expression(0)));
        expr.add("rhs", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitNotExpression(SmallPearlParser.NotExpressionContext ctx) {
        TypeDefinition typ = m_expressionTypeVisitor.lookupType(ctx);
        ST expr = null;

        // TODO: bitwise
        if ( typ instanceof TypeBit) {
            TypeBit b = (TypeBit)typ;
            expr = m_group.getInstanceOf("NotBitwiseExpression");
        } else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        expr.add("rhs", visit(ctx.expression()));

        return expr;
    }

    @Override
    public ST visitAndExpression(SmallPearlParser.AndExpressionContext ctx) {
        TypeDefinition typ = m_expressionTypeVisitor.lookupType(ctx);
        ST expr = null;

        if ( typ instanceof TypeBit) {
            TypeBit b = (TypeBit)typ;
            expr = m_group.getInstanceOf("AndBitwiseExpression");
        } else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        expr.add("lhs", visit(ctx.expression(0)));
        expr.add("rhs", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitOrExpression(SmallPearlParser.OrExpressionContext ctx) {
        TypeDefinition typ = m_expressionTypeVisitor.lookupType(ctx);
        ST expr = null;

        if ( typ instanceof TypeBit) {
            TypeBit b = (TypeBit)typ;
            expr = m_group.getInstanceOf("OrBitwiseExpression");
        } else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        expr.add("lhs", visit(ctx.expression(0)));
        expr.add("rhs", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitExorExpression(SmallPearlParser.ExorExpressionContext ctx) {
        TypeDefinition typ = m_expressionTypeVisitor.lookupType(ctx);
        ST expr = null;

        if ( typ instanceof TypeBit) {
            TypeBit b = (TypeBit)typ;
            expr = m_group.getInstanceOf("ExorBitwiseExpression");
        } else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        expr.add("lhs", visit(ctx.expression(0)));
        expr.add("rhs", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitCshiftExpression(SmallPearlParser.CshiftExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("CshiftExpression");

        expr.add("lhs", visit(ctx.expression(0)));
        expr.add("rhs", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitShiftExpression(SmallPearlParser.ShiftExpressionContext ctx) {
        ST expr = m_group.getInstanceOf("ShiftExpression");

        expr.add("lhs", visit(ctx.expression(0)));
        expr.add("rhs", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitCatExpression(SmallPearlParser.CatExpressionContext ctx) {
        ST st;

        TypeDefinition resultType = m_expressionTypeVisitor.lookupType(ctx);
        TypeDefinition op1Type = m_expressionTypeVisitor.lookupType(ctx.expression(0));
        TypeDefinition op2Type = m_expressionTypeVisitor.lookupType(ctx.expression(1));


        if ( resultType instanceof TypeChar) {
            st = m_group.getInstanceOf("CharCatExpression");

        }
        else if ( resultType instanceof TypeBit) {
            st = m_group.getInstanceOf("BitCatExpression");
        }
        else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        st.add("op1", visit(ctx.expression(0)));
        st.add("op2", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitLiteral(SmallPearlParser.LiteralContext ctx) {
        ST literal = m_group.getInstanceOf("literal");

        if (ctx.durationConstant() != null) {
            ConstantDurationValue duration = getConstantDurationValue(ctx.durationConstant());
            ConstantDurationValue constDuration = ConstantPool.lookupDurationValue(duration.getHours(),duration.getMinutes(),duration.getSeconds());

            if (constDuration != null) {
                literal.add("duration", constDuration);
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.floatingPointConstant() != null) {
            try {
                Double value = Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
                int precision = m_currentSymbolTable.lookupDefaultFloatLength();
                ConstantFloatValue constFloat = ConstantPool.lookupFloatValue(value,precision);

                if (constFloat != null) {
                    literal.add("float", constFloat);
                } else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.timeConstant() != null) {
            literal.add("time", getTime(ctx.timeConstant()));
        } else if (ctx.StringLiteral() != null) {
            String s = ctx.StringLiteral().getText();
            s = CommonUtils.remopveQuotes(s);
            ST constantCharacterValue = m_group.getInstanceOf("ConstantCharacterValue");
            ConstantCharacterValue value = ConstantPool.lookupCharacterValue(s);

            if (value != null) {
                literal.add("string", value);
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if ( ctx.BitStringLiteral() != null ) {
            String s = ctx.BitStringLiteral().getText();
            long value = CommonUtils.convertBitStringToLong(ctx.BitStringLiteral().getText());
            int  nb = CommonUtils.getBitStringLength(ctx.BitStringLiteral().getText());
            ConstantBitValue bitStr = ConstantPool.lookupBitValue(value,nb);

            if (bitStr != null) {
                literal.add("bitstring", bitStr);
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.fixedConstant() != null) {
            try {
                int value;
                int precision = m_currentSymbolTable.lookupDefaultFixedLength();

                if ( ctx.fixedConstant().fixedNumberPrecision() != null) {
                    precision = Integer.parseInt(ctx.fixedConstant().fixedNumberPrecision().IntegerConstant().toString());
                } else {
                    if (m_currFixedLength != null ) {
                        precision = m_currFixedLength;
                    }
                }

                String s = ctx.fixedConstant().IntegerConstant().toString();

                value = Integer.parseInt(ctx.fixedConstant().IntegerConstant().toString());

                if (m_map_to_const) {
                    ConstantFixedValue fixed_value = new ConstantFixedValue(value,precision);
                    literal.add("integer", fixed_value);
                }
                else {
                    literal.add("integer", value);
                }
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return literal;
    }

    @Override
    public ST visitReturnStatement(SmallPearlParser.ReturnStatementContext ctx) {
        ST stmt = m_group.getInstanceOf("return_statement");
        ST returnExpr = getExpression(ctx.expression());

        if ( returnExpr != null ) {
            stmt.add("expression", returnExpr);
        }

        return stmt;
    }

    @Override
    public ST visitTask_control_statement(SmallPearlParser.Task_control_statementContext ctx) {
        ST stmt = m_group.getInstanceOf("task_control_statement");

        if (ctx.taskStart() != null) {
            stmt.add("code", visitTaskStart(ctx.taskStart()));
        } else if (ctx.task_terminating() != null) {
            stmt.add("code", visitTask_terminating(ctx.task_terminating()));
        } else if (ctx.task_suspending() != null) {
            stmt.add("code", visitTask_suspending(ctx.task_suspending()));
        } else if (ctx.taskContinuation() != null) {
            stmt.add("code", visitTaskContinuation(ctx.taskContinuation()));
        } else if (ctx.taskResume() != null) {
            stmt.add("code", visitTaskResume(ctx.taskResume()));
        } else if (ctx.task_preventing() != null) {
            stmt.add("code", visitTask_preventing(ctx.task_preventing()));
        }

        return stmt;
    }

    @Override
    public ST visitTaskStart(SmallPearlParser.TaskStartContext ctx) {
        ST st = m_group.getInstanceOf("task_start");

        st.add("task", ctx.ID().toString());

        if (ctx.startCondition() instanceof SmallPearlParser.StartConditionATContext) {
            SmallPearlParser.StartConditionATContext c = (SmallPearlParser.StartConditionATContext) ctx.startCondition();
            st.add("Condition", "AT");
            if (c.expression() != null) {
                st.add("at", getExpression(c.expression()));
            }
        } else if (ctx.startCondition() instanceof SmallPearlParser.StartConditionAFTERContext) {
            SmallPearlParser.StartConditionAFTERContext c = (SmallPearlParser.StartConditionAFTERContext) ctx.startCondition();
            st.add("Condition", "AFTER");
            if (c.expression() != null) {
                st.add("after", getExpression(c.expression()));
            }
        }

        if (ctx.frequency() != null) {
            SmallPearlParser.FrequencyContext c = ctx.frequency();
            st.add("Condition", "ALL");
            st.add("all",getExpression(c.expression(0)));

            for (int i = 0; i < c.getChildCount(); i++) {
                if (c.getChild(i) instanceof TerminalNodeImpl) {
                    if (((TerminalNodeImpl) c.getChild(i)).getSymbol().getText().equals("UNTIL")) {
                        st.add("Condition", "UNTIL");
                        st.add("until", getExpression(c.expression(1)));
                    } else if (((TerminalNodeImpl) c.getChild(i)).getSymbol().getText().equals("DURING")) {
                        st.add("Condition", "DURING");
                        st.add("during", getExpression(c.expression(1)));
                    }
                }
            }
        }

        if (ctx.priority() != null) {
            st.add("Condition", "PRIO");
            st.add("priority", getExpression(ctx.priority().expression()));
        }

        return st;
    }

    @Override
    public ST visitTask_terminating(SmallPearlParser.Task_terminatingContext ctx) {
        ST stmt = m_group.getInstanceOf("task_terminate");

        if (ctx.ID() != null) {
            stmt.add("task", ctx.ID().toString());
        }

        return stmt;
    }

    @Override
    public ST visitTask_suspending(SmallPearlParser.Task_suspendingContext ctx) {
        ST stmt = m_group.getInstanceOf("task_suspend");

        if (ctx.ID() != null) {
            stmt.add("task", ctx.ID().toString());
        }

        return stmt;
    }

    @Override
    public ST visitTaskContinuation(SmallPearlParser.TaskContinuationContext ctx) {
        ST st = m_group.getInstanceOf("TaskContinuation");

        if (ctx.ID() != null) {
            st.add("task", ctx.ID().toString());
        }

        if (ctx.startCondition() instanceof SmallPearlParser.StartConditionATContext) {
            SmallPearlParser.StartConditionATContext c = (SmallPearlParser.StartConditionATContext) ctx.startCondition();
            st.add("Condition", "AT");
            if (c.expression() != null) {
                st.add("at", getExpression(c.expression()));
            }
        } else if (ctx.startCondition() instanceof SmallPearlParser.StartConditionAFTERContext) {
            SmallPearlParser.StartConditionAFTERContext c = (SmallPearlParser.StartConditionAFTERContext) ctx.startCondition();
            st.add("Condition", "AFTER");
            if (c.expression() != null) {
                st.add("after", getExpression(c.expression()));
            }
        }

        if (ctx.priority() != null) {
            st.add("Condition", "PRIO");
            st.add("priority", getExpression(ctx.priority().expression()));
        }

        return st;
    }

    @Override
    public ST visitTaskResume(SmallPearlParser.TaskResumeContext ctx) {
        ST st = m_group.getInstanceOf("TaskResume");

        if (ctx.startCondition() instanceof SmallPearlParser.StartConditionATContext) {
            SmallPearlParser.StartConditionATContext c = (SmallPearlParser.StartConditionATContext) ctx.startCondition();
            st.add("Condition", "AT");
            if (c.expression() != null) {
                st.add("at", getExpression(c.expression()));
            }
        } else if (ctx.startCondition() instanceof SmallPearlParser.StartConditionAFTERContext) {
            SmallPearlParser.StartConditionAFTERContext c = (SmallPearlParser.StartConditionAFTERContext) ctx.startCondition();
            st.add("Condition", "AFTER");
            if (c.expression() != null) {
                st.add("after", getExpression(c.expression()));
            }
        }

        return st;
    }

    @Override
    public ST visitTask_preventing(SmallPearlParser.Task_preventingContext ctx) {
        ST stmt = m_group.getInstanceOf("task_prevent");
        stmt.add("task", ctx.ID().toString());
        return stmt;
    }

    @Override
    public ST visitTask_coordination_statement(SmallPearlParser.Task_coordination_statementContext ctx) {
        ST stmt = m_group.getInstanceOf("TaskCoordinationStatement");

        if (ctx.semaRelease() != null) {
            stmt.add("statement", visitSemaRelease(ctx.semaRelease()));
        } else if (ctx.semaRequest() != null) {
            stmt.add("statement", visitSemaRequest(ctx.semaRequest()));
        } else if (ctx.boltEnter() != null) {
            stmt.add("statement", visitBoltEnter(ctx.boltEnter()));
        } else if (ctx.boltReserve() != null) {
            stmt.add("statement", visitBoltReserve(ctx.boltReserve()));
        } else if (ctx.boltFree() != null) {
            stmt.add("statement", visitBoltFree(ctx.boltFree()));
        } else if (ctx.boltLeave() != null) {
            stmt.add("statement", visitBoltLeave(ctx.boltLeave()));
        }
        return stmt;
    }

    @Override
    public ST visitSemaTry(SmallPearlParser.SemaTryContext ctx) {
        ST st = m_group.getInstanceOf("SemaTry");
        LinkedList<String> listOfNames = new LinkedList<String>();

        for (int i = 0; i < ctx.ID().size(); i++) {
            listOfNames.add(ctx.ID(i).getText());
        }

        Collections.sort(listOfNames);

        for (int i = 0; i < listOfNames.size(); i++) {
            st.add("names", listOfNames.get(i));
        }

        st.add("noofsemas", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitSemaRelease(SmallPearlParser.SemaReleaseContext ctx) {
        ST st = m_group.getInstanceOf("SemaRelease");
        LinkedList<String> listOfNames = new LinkedList<String>();

        for (int i = 0; i < ctx.ID().size(); i++) {
            listOfNames.add(ctx.ID(i).getText());
        }

        Collections.sort(listOfNames);

        for (int i = 0; i < listOfNames.size(); i++) {
            st.add("names", listOfNames.get(i));
        }

        st.add("noofsemas", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitSemaRequest(SmallPearlParser.SemaRequestContext ctx) {
        ST st = m_group.getInstanceOf("SemaRequest");
        LinkedList<String> listOfNames = new LinkedList<String>();

        for (int i = 0; i < ctx.ID().size(); i++) {
            listOfNames.add(ctx.ID(i).getText());
        }

        Collections.sort(listOfNames);

        for (int i = 0; i < listOfNames.size(); i++) {
            st.add("names", listOfNames.get(i));
        }

        st.add("noofsemas", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitConstant(SmallPearlParser.ConstantContext ctx) {
        ST st = m_group.getInstanceOf("Constant");
        return st;
    }

    @Override
    public ST visitIo_statement(SmallPearlParser.Io_statementContext ctx) {
        ST stmt = m_group.getInstanceOf("io_statement");

        if (ctx.close_statement() != null) {
            stmt.add("code", visitClose_statement(ctx.close_statement()));
        } else if (ctx.open_statement() != null) {
            stmt.add("code", visitOpen_statement(ctx.open_statement()));
        } else if (ctx.readStatement() != null) {
            stmt.add("code", visitReadStatement(ctx.readStatement()));
        } else if (ctx.sendStatement() != null) {
            stmt.add("code", visitSendStatement(ctx.sendStatement()));
        } else if (ctx.takeStatement() != null) {
            stmt.add("code", visitTakeStatement(ctx.takeStatement()));
        } else if (ctx.writeStatement() != null) {
            stmt.add("code", visitWriteStatement(ctx.writeStatement()));
        } else if (ctx.getStatement() != null) {
            stmt.add("code", visitGetStatement(ctx.getStatement()));
        } else if (ctx.putStatement() != null) {
            stmt.add("code", visitPutStatement(ctx.putStatement()));
        }

        return stmt;
    }

    @Override
    public ST visitClose_statement(SmallPearlParser.Close_statementContext ctx) {
        ST stmt = m_group.getInstanceOf("close_statement");
        stmt.add("id", ctx.ID());

        if (ctx.close_parameterlist() != null) {
            stmt.add("paramlist", visitClose_parameterlist(ctx.close_parameterlist()));

            ArrayList<String> rstVars = getCloseRstVariables(ctx.close_parameterlist());

            if (rstVars.size() > 1) {
                throw new NotSupportedFeatureException("close_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine(), "CLOSE: Mulitple RST not supported");
            }

            if (rstVars.size() == 1) {
                String var = rstVars.get(0).toString();

                stmt.add("rst_var", var);
            }
        }
        else {
            ST st = m_group.getInstanceOf("close_parameterlist");
            st.add("parameter", m_group.getInstanceOf("close_parameter_none"));
            stmt.add("paramlist", st);
            stmt.add("rst_var", m_group.getInstanceOf("close_parameter_no_rst"));
        }

        return stmt;
    }

    private boolean isRSTSpecified(SmallPearlParser.Close_parameterlistContext ctx) {
        boolean rstFound = false;

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Close_parameter_rstContext) {
                rstFound = true;
                break;
            }
        }

        return rstFound;
    }

    @Override
    public ST visitOpen_statement(SmallPearlParser.Open_statementContext ctx) {
        ST stmt = m_group.getInstanceOf("open_statement");

        stmt.add("id", ctx.ID());
        if (ctx.open_parameterlist() != null) {
            stmt.add("paramlist", visitOpen_parameterlist(ctx.open_parameterlist()));

            ArrayList<String> idfFilenames = getIdfFilenames(ctx.open_parameterlist());

            if (idfFilenames.size() > 1) {
                throw new NotSupportedFeatureException("open_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine(), "OPEN: Mulitple IDF not supported");
            }

            if (idfFilenames.size() == 1) {
                String fname = idfFilenames.get(0).toString();
//                fname = fname.substring(1, fname.length() - 1);

                ST declFilename = m_group.getInstanceOf("declare_idf_filename");
                ST refFilename = m_group.getInstanceOf("reference_idf_filename");

                SymbolTableEntry entry = m_currentSymbolTable.lookup(fname);

                if ( entry instanceof VariableEntry ) {
                    declFilename.add("variable",fname);
                    refFilename.add("variable",fname);
                }
                else {
                    declFilename.add("stringConstant",fname);
                    declFilename.add("lengthOfStringConstant", fname.length());
                    refFilename.add("stringConstant",fname);
                }

                stmt.add("declFileName", declFilename);
                stmt.add("refFileName", refFilename);

            }

            ArrayList<String> rstVars = getOpenRstVariables(ctx.open_parameterlist());

            if (rstVars.size() > 1) {
                throw new NotSupportedFeatureException("open_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine(), "OPEN: Mulitple RST not supported");
            }

            if (rstVars.size() == 1) {
                String var = rstVars.get(0).toString();

                stmt.add("rst_var", var);
            }
        }

        return stmt;
    }

    private ArrayList<String> getIdfFilenames(SmallPearlParser.Open_parameterlistContext ctx) {
        ArrayList<String> filenames = new ArrayList<String>();

        if (ctx != null) {
            for (int i = 0; i < ctx.open_parameter().size(); i++) {
                if (ctx.open_parameter(i) instanceof SmallPearlParser.Open_parameter_idfContext) {
                    SmallPearlParser.Open_parameter_idfContext c = (SmallPearlParser.Open_parameter_idfContext) ctx.open_parameter(i);

                    if (c.StringLiteral() != null) {
                        String quoteless = c.StringLiteral().toString().subSequence(1, c.StringLiteral().toString().length() - 1).toString();
                        filenames.add(quoteless);
                    } else if (c.ID() != null) {
                        SymbolTableEntry entry = m_currentSymbolTable.lookup(c.ID().getText());

                        if ( !(entry instanceof VariableEntry )) {
                            throw new UnknownIdentifierException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                        }


                        filenames.add(c.ID().toString());
                    }
                }

            }
        }

        return filenames;
    }

    private ArrayList<String> getOpenRstVariables(SmallPearlParser.Open_parameterlistContext ctx) {
        ArrayList<String> vars = new ArrayList<String>();

        if (ctx != null) {
            for (int i = 0; i < ctx.open_parameter().size(); i++) {
                if (ctx.open_parameter(i) instanceof SmallPearlParser.Open_parameter_rstContext) {
                    SmallPearlParser.Open_parameter_rstContext c = (SmallPearlParser.Open_parameter_rstContext) ctx.open_parameter(i);

                    if (c.ID() != null) {
                        vars.add(c.ID().toString());
                    }
                }

            }
        }

        return vars;
    }

    private ArrayList<String> getCloseRstVariables(SmallPearlParser.Close_parameterlistContext ctx) {
        ArrayList<String> vars = new ArrayList<String>();

        if (ctx != null) {
            for (int i = 0; i < ctx.close_parameter().size(); i++) {
                if (ctx.close_parameter(i) instanceof SmallPearlParser.Close_parameter_rstContext) {
                    SmallPearlParser.Close_parameter_rstContext c = (SmallPearlParser.Close_parameter_rstContext) ctx.close_parameter(i);

                    if (c.ID() != null) {
                        vars.add(c.ID().toString());
                    }
                }
            }
        }

        return vars;
    }

    @Override
    public ST visitOpen_parameterlist(SmallPearlParser.Open_parameterlistContext ctx) {
        ST st = m_group.getInstanceOf("open_parameterlist");

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Open_parameter_old_or_new_or_anyContext) {
                SmallPearlParser.Open_parameter_old_or_new_or_anyContext ctxTmp = (SmallPearlParser.Open_parameter_old_or_new_or_anyContext) c;
                if (ctxTmp.open_parameter_old_new_any() != null) {
                    st.add("parameter", visitOpen_parameter_old_or_new_or_any(ctxTmp));
                }
            } else if (c instanceof SmallPearlParser.Open_parameter_can_or_prmContext) {
                SmallPearlParser.Open_parameter_can_or_prmContext ctxTmp = (SmallPearlParser.Open_parameter_can_or_prmContext) c;
                if (ctxTmp.open_parameter_can_prm() != null) {
                    st.add("parameter", visitOpen_parameter_can_or_prm(ctxTmp));
                }
            } else if (c instanceof SmallPearlParser.Open_parameter_rstContext) {
                SmallPearlParser.Open_parameter_rstContext ctxTmp = (SmallPearlParser.Open_parameter_rstContext) c;
                ST stTmp = m_group.getInstanceOf("open_parameter_rst");

                if (ctxTmp.ID() != null) {
                    stTmp.add("id", ctxTmp.ID().getText());
                }

                st.add("parameter", stTmp);
            } else if (c instanceof SmallPearlParser.Open_parameter_idfContext) {
                SmallPearlParser.Open_parameter_idfContext ctxTmp = (SmallPearlParser.Open_parameter_idfContext) c;
                st.add("parameter", visitOpen_parameter_idf(ctxTmp));
            }

        }

        return st;
    }

    @Override
    public ST visitOpen_parameter_idf(SmallPearlParser.Open_parameter_idfContext ctx) {
        ST st = m_group.getInstanceOf("open_parameter_idf");

        if (ctx.ID() != null) {
            st.add("id", ctx.ID().getText());
        } else if (ctx.StringLiteral() != null) {
            st.add("string", ctx.StringLiteral().getText());
        }
        return st;
    }

    @Override
    public ST visitOpen_parameter_old_or_new_or_any(SmallPearlParser.Open_parameter_old_or_new_or_anyContext ctx) {
        if (ctx.open_parameter_old_new_any() instanceof SmallPearlParser.Open_parameter_oldContext) {
            ST st = m_group.getInstanceOf("open_parameter_old");
            st.add("attribute", 1);
            return st;
        } else if (ctx.open_parameter_old_new_any() instanceof SmallPearlParser.Open_parameter_newContext) {
            ST st = m_group.getInstanceOf("open_parameter_new");
            st.add("attribute", 1);
            return st;
        } else if (ctx.open_parameter_old_new_any() instanceof SmallPearlParser.Open_parameter_anyContext) {
            ST st = m_group.getInstanceOf("open_parameter_any");
            st.add("attribute", 1);
            return st;
        }

        return null;
    }


    @Override
    public ST visitClose_parameterlist(SmallPearlParser.Close_parameterlistContext ctx) {
        ST st = m_group.getInstanceOf("close_parameterlist");

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Close_parameter_canContext) {
                ST can = m_group.getInstanceOf("close_parameter_can");
                can.add("attribute", 1);
                st.add("parameter", can);
            } else if (c instanceof SmallPearlParser.Close_parameter_prmContext) {
                ST prm = m_group.getInstanceOf("close_parameter_prm");
                prm.add("attribute", 1);
                st.add("parameter", prm);
            } else if (c instanceof SmallPearlParser.Close_parameter_rstContext) {
                ST rst = m_group.getInstanceOf("close_parameter_rst");
                rst.add("id", ((SmallPearlParser.Close_parameter_rstContext) c).ID());
                st.add("parameter", rst);
            }
        }

        return st;
    }

    @Override
    public ST visitGetStatement(SmallPearlParser.GetStatementContext ctx) {
        ST stmt = m_group.getInstanceOf("get_statement");

        String dation = ctx.getFrom().ID().getText();

        stmt.add("dation", getUserVariable(dation));

        int j = 0;
        for (int i = 0; i < ctx.ID().size(); i++) {
            if (ctx.formatPosition() != null) {
                int k = 0;
                boolean foundFormat = false;
                while ( k < ctx.formatPosition().size() && !foundFormat) {
                    if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatContext) {
                        ST e = getFactorFormatForGet((SmallPearlParser.FactorFormatContext) ctx.formatPosition(j), dation, ctx.ID(i).getText());
                        stmt.add("elements", e);
                        foundFormat = true;
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorPositionContext) {
                        // TODO: visitExpression???
                        ST e = getFactorPositionForGet((SmallPearlParser.FactorPositionContext) ctx.formatPosition(j), dation, ctx.ID(i).getText());
                        stmt.add("elements", e);
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatPositionContext) {
                    }

                    k = k + 1;
                    j = j + 1;
                    if (j == ctx.formatPosition().size()) {
                        j = 0;
                    }
                }

                if ( !foundFormat) {
                    throw new NoOfFormatSpecifiersDoesNotMatchException("get_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
        }

        if (ctx.formatPosition() != null) {
            for(int k = j; k < ctx.formatPosition().size(); k++) {
                if (ctx.formatPosition(k) instanceof SmallPearlParser.FactorPositionContext) {
                    ST e = getFactorPositionForGet((SmallPearlParser.FactorPositionContext) ctx.formatPosition(k), dation, null);
                    stmt.add("elements", e);
                }
            }
        }

        return stmt;
    }

    @Override
    public ST visitPutStatement(SmallPearlParser.PutStatementContext ctx) {
        ST stmt = m_group.getInstanceOf("put_statement");
        stmt.add("dation", getUserVariable(ctx.ID().toString()));

        int j = 0;

        int formatCount = 0;
        for ( int i = 0; i < ctx.formatPosition().size(); i++ ) {
            if (ctx.formatPosition(i) instanceof SmallPearlParser.FactorFormatContext) {
                formatCount++;
            }
        }

        if ( formatCount > ctx.expression().size() ) {
            throw new NoOfFormatSpecifiersDoesNotMatchException("put_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        for (int i = 0; i < ctx.expression().size(); i++) {
            if (ctx.formatPosition() != null) {
                int k = 0;
                boolean foundFormat = false;

                while ( k < ctx.formatPosition().size() && !foundFormat) {
                    if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatContext) {
                        ST e = getFactorFormatForPut((SmallPearlParser.FactorFormatContext) ctx.formatPosition(j), ctx.ID().getText(), ctx.expression(i));
                        stmt.add("elements", e);
                        foundFormat = true;
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorPositionContext) {
                        // TODO: visitExpression???
                        ST e = getFactorPositionForPut((SmallPearlParser.FactorPositionContext) ctx.formatPosition(j), ctx.ID().getText(), ctx.expression(i));
                        stmt.add("elements", e);
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatPositionContext) {
                    }

                    k = k + 1;
                    j = j + 1;
                    if (j == ctx.formatPosition().size()) {
                        j = 0;
                    }
                }

                if ( !foundFormat) {
                    throw new NoOfFormatSpecifiersDoesNotMatchException("put_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }

        }

        if (ctx.formatPosition() != null) {
            for(int k = j; k < ctx.formatPosition().size(); k++) {
                if (ctx.formatPosition(k) instanceof SmallPearlParser.FactorPositionContext) {
                    ST e = getFactorPositionForPut((SmallPearlParser.FactorPositionContext) ctx.formatPosition(k), ctx.ID().getText(), null);
                    stmt.add("elements", e);
                }
            }
        }

        return stmt;
    }

    private ST getFactorFormatForPut(SmallPearlParser.FactorFormatContext ctx, String dation, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_factor_format");

        st.add("dation", getUserVariable(dation));

        if ( ctx.format().fixedFormat() != null ) {
            st.add("format", getFixedFormatForPut(ctx.format().fixedFormat(), expression));
        }
        else if ( ctx.format().floatFormat() != null ) {
            if ( ctx.format().floatFormat() instanceof SmallPearlParser.FloatFormatEContext ) {
               st.add("format", getFloatFormatEForPut((SmallPearlParser.FloatFormatEContext)ctx.format().floatFormat(), expression));
            }
            else if ( ctx.format().floatFormat() instanceof SmallPearlParser.FloatFormatE3Context ) {
                st.add("format", getFloatFormatE3ForPut((SmallPearlParser.FloatFormatE3Context)ctx.format().floatFormat(), expression));
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
        else if ( ctx.format().bitFormat() != null ) {
            if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat1Context) {
                st.add("format", getBitFormat1ForPut((SmallPearlParser.BitFormat1Context)ctx.format().bitFormat(), expression));
            }
            else if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat2Context) {
                st.add("format", getBitFormat2ForPut((SmallPearlParser.BitFormat2Context)ctx.format().bitFormat(), expression));
            }
            else if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat3Context) {
                st.add("format", getBitFormat3ForPut((SmallPearlParser.BitFormat3Context)ctx.format().bitFormat(), expression));
            }
            else if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat4Context) {
                st.add("format", getBitFormat4ForPut((SmallPearlParser.BitFormat4Context)ctx.format().bitFormat(), expression));
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
        else if ( ctx.format().durationFormat() != null ) {
            st.add("format", getDurationFormatForPut(ctx.format().durationFormat(), expression));
        }
        else if ( ctx.format().timeFormat() != null ) {
            st.add("format", getTimeFormatForPut(ctx.format().timeFormat(), expression));
        }
        else {
            st.add("format", getCharacterStringFormatForPut(ctx.format().characterStringFormat(), expression));
        }

        return st;
    }

    private ST getFactorFormatForGet(SmallPearlParser.FactorFormatContext ctx, String dation, String element) {
        ST st = m_group.getInstanceOf("get_statement_factor_format");

        st.add("dation", getUserVariable(dation));

        if ( ctx.format().fixedFormat() != null ) {
            st.add("format", getFixedFormatForGet(ctx.format().fixedFormat(), element));
        }
        else if ( ctx.format().floatFormat() != null ) {
            if ( ctx.format().floatFormat() instanceof SmallPearlParser.FloatFormatEContext ) {
                st.add("format", getFloatFormatEForGet((SmallPearlParser.FloatFormatEContext)ctx.format().floatFormat(), element));
            }
            else if ( ctx.format().floatFormat() instanceof SmallPearlParser.FloatFormatE3Context ) {
                st.add("format", getFloatFormatE3ForGet((SmallPearlParser.FloatFormatE3Context)ctx.format().floatFormat(), element));
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
        else if ( ctx.format().bitFormat() != null ) {
            if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat1Context) {
                st.add("format", getBitFormat1ForGet((SmallPearlParser.BitFormat1Context)ctx.format().bitFormat(), element));
            }
            else if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat2Context) {
                st.add("format", getBitFormat2ForGet((SmallPearlParser.BitFormat2Context)ctx.format().bitFormat(), element));
            }
            else if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat3Context) {
                st.add("format", getBitFormat3ForGet((SmallPearlParser.BitFormat3Context)ctx.format().bitFormat(), element));
            }
            else if ( ctx.format().bitFormat() instanceof SmallPearlParser.BitFormat4Context) {
                st.add("format", getBitFormat4ForGet((SmallPearlParser.BitFormat4Context)ctx.format().bitFormat(), element));
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        else if ( ctx.format().durationFormat() != null ) {
            st.add("format", getDurationFormatForGet(ctx.format().durationFormat(), element));
        }
        else if ( ctx.format().timeFormat() != null ) {
            st.add("format", getTimeFormatForGet(ctx.format().timeFormat(), element));
        }
        else {
            st.add("format", getCharacterStringFormatForGet(ctx.format().characterStringFormat(), element));
        }



        return st;
    }

    private ST getCharacterStringFormatForPut(SmallPearlParser.CharacterStringFormatContext ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_character_string_format");

        st.add("element", getExpression(expression));

        if( ctx instanceof SmallPearlParser.CharacterStringFormatAContext ) {
            SmallPearlParser.CharacterStringFormatAContext c = (SmallPearlParser.CharacterStringFormatAContext) ctx;

            if ( c.expression() != null ) {
                st.add("fieldwidth", getExpression(c.expression()));
            }
        }

        return st;
    }

    private ST getCharacterStringFormatForGet(SmallPearlParser.CharacterStringFormatContext ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_character_string_format");
        st.add("element", getUserVariable(element));

        if( ctx instanceof SmallPearlParser.CharacterStringFormatAContext ) {
            SmallPearlParser.CharacterStringFormatAContext c = (SmallPearlParser.CharacterStringFormatAContext) ctx;

            if ( c.expression() != null ) {
                st.add("fieldwidth", getExpression(c.expression()));
            }
        }

        return st;
    }

    private ST getFixedFormatForPut(SmallPearlParser.FixedFormatContext ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_fixed_format");

        st.add("element", getExpression(expression));
        st.add( "fieldwidth",  getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));

            if ( ctx.scaleFactor() != null ) {
                st.add( "scaleFactor", getExpression(ctx.scaleFactor().expression()));
            }
        }

        return st;
    }

    private ST getFloatFormatEForPut(SmallPearlParser.FloatFormatEContext ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_float_format_E");

        st.add("element", getExpression(expression));
        st.add("fieldwidth", getExpression(ctx.fieldWidth().expression()));
        st.add("esize", 2);

        if ( ctx.decimalPositions() != null ) {
            st.add("decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }
        else {
            st.add("decimalPositions", 15);
        }

        if ( ctx.expression() != null ) {
            st.add( "significance", getExpression(ctx.expression()));
        }
        else {
            st.add("significance", 15);
        }

        return st;
    }

    private ST getFloatFormatE3ForPut(SmallPearlParser.FloatFormatE3Context ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_float_format_E");

        st.add( "element", getExpression(expression));
        st.add( "fieldwidth",  getExpression(ctx.fieldWidth().expression()));
        st.add( "esize", 3);

        if ( ctx.decimalPositions() != null ) {
            st.add("decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }
        else {
            st.add("decimalPositions", 15);
        }

        if ( ctx.expression() != null ) {
            st.add( "significance", getExpression(ctx.expression()));
        }
        else {
            st.add("significance", 15);
        }

        return st;
    }

    private ST getBitFormat1ForPut(SmallPearlParser.BitFormat1Context ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_bit_format_1");

        st.add("element", getExpression(expression));

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }

    private ST getBitFormat2ForPut(SmallPearlParser.BitFormat2Context ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_bit_format_2");

        st.add("element", getExpression(expression));

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }
    private ST getBitFormat3ForPut(SmallPearlParser.BitFormat3Context ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_bit_format_3");

        st.add("element", getExpression(expression));

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }
    private ST getBitFormat4ForPut(SmallPearlParser.BitFormat4Context ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_bit_format_4");

        st.add("element", getExpression(expression));

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }

    private ST getDurationFormatForPut(SmallPearlParser.DurationFormatContext ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_duration_format");

        st.add("element", getExpression(expression));
        st.add( "fieldwidth", getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }

        return st;
    }

    private ST getTimeFormatForPut(SmallPearlParser.TimeFormatContext ctx, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_time_format");

        st.add("element", getExpression(expression));
        st.add( "fieldwidth", getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }

        return st;
    }

    private ST getFixedFormatForGet(SmallPearlParser.FixedFormatContext ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_fixed_format");

        st.add("element", getUserVariable(element));
        st.add( "fieldwidth", getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));

            if ( ctx.scaleFactor() != null ) {
                st.add( "scaleFactor", getExpression(ctx.scaleFactor().expression()));
            }
        }

        return st;
    }
    private ST getFloatFormatEForGet(SmallPearlParser.FloatFormatEContext ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_float_format_E");

        st.add("element", element);
        st.add("fieldwidth", getExpression(ctx.fieldWidth().expression()));
        st.add("esize", 2);

        if ( ctx.decimalPositions() != null ) {
            st.add("decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }
        else {
            st.add("decimalPositions", 15);
        }

        if ( ctx.expression() != null ) {
            st.add( "significance", getExpression(ctx.expression()));
        }
        else {
            st.add("significance", 15);
        }

        return st;
    }

    private ST getFloatFormatE3ForGet(SmallPearlParser.FloatFormatE3Context ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_float_format_E");

        st.add( "element", element);
        st.add( "fieldwidth",  getExpression(ctx.fieldWidth().expression()));
        st.add( "esize", 3);

        if ( ctx.decimalPositions() != null ) {
            st.add("decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }
        else {
            st.add("decimalPositions", 15);
        }

        if ( ctx.expression() != null ) {
            st.add( "significance", getExpression(ctx.expression()));
        }
        else {
            st.add("significance", 15);
        }

        return st;
    }


    private ST getBitFormat1ForGet(SmallPearlParser.BitFormat1Context ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_bit_format_1");

        st.add("element", element);

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }

    private ST getBitFormat2ForGet(SmallPearlParser.BitFormat2Context ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_bit_format_2");

        st.add("element", element);

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }
    private ST getBitFormat3ForGet(SmallPearlParser.BitFormat3Context ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_bit_format_3");

        st.add("element", element);

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }
    private ST getBitFormat4ForGet(SmallPearlParser.BitFormat4Context ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_bit_format_4");

        st.add("element", element);

        if ( ctx.numberOfCharacters() != null && ctx.numberOfCharacters().expression() != null ) {
            st.add("numberOfCharacters", getExpression(ctx.numberOfCharacters().expression()));
        }

        return st;
    }

    private ST getDurationFormatForGet(SmallPearlParser.DurationFormatContext ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_duration_format");

        st.add("element", element);
        st.add( "fieldwidth", getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }

        return st;
    }

    private ST getTimeFormatForGet(SmallPearlParser.TimeFormatContext ctx, String element) {
        ST st = m_group.getInstanceOf("get_statement_time_format");

        st.add("element", element);
        st.add( "fieldwidth", getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));
        }

        return st;
    }



    private ST getFactorPositionForPut(SmallPearlParser.FactorPositionContext ctx, String dation, SmallPearlParser.ExpressionContext expression) {
        ST st = m_group.getInstanceOf("put_statement_factor_position");
        st.add("dation", getUserVariable(dation));

        if ( ctx.position() instanceof  SmallPearlParser.PositionRSTContext ) {
            SmallPearlParser.PositionRSTContext c = (SmallPearlParser.PositionRSTContext) ctx.position();
            ST e = m_group.getInstanceOf("put_statement_factor_position_rst");
            e.add("element", getUserVariable(c.ID().getText()));
            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionSKIPContext ) {
            SmallPearlParser.PositionSKIPContext c = (SmallPearlParser.PositionSKIPContext) ctx.position();
            ST e = m_group.getInstanceOf("put_statement_factor_position_skip");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", "CONST_FIXED_P_1_" + Defaults.FIXED_LENGTH);
            }

            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionXContext ) {
            SmallPearlParser.PositionXContext c = (SmallPearlParser.PositionXContext) ctx.position();
            ST e = m_group.getInstanceOf("put_statement_factor_position_x");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", 1);
            }

            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionEOFContext ) {
            SmallPearlParser.PositionEOFContext c = (SmallPearlParser.PositionEOFContext) ctx.position();
            ST e = m_group.getInstanceOf("put_statement_factor_position_eof");
        }

        return st;
    }

    private ST getFactorPositionForGet(SmallPearlParser.FactorPositionContext ctx, String dation, String element) {
        ST st = m_group.getInstanceOf("get_statement_factor_position");
        st.add("dation", getUserVariable(dation));
//TODO:

        if ( ctx.position() instanceof  SmallPearlParser.PositionRSTContext ) {
            SmallPearlParser.PositionRSTContext c = (SmallPearlParser.PositionRSTContext) ctx.position();
            ST e = m_group.getInstanceOf("get_statement_factor_position_rst");
            e.add("element", getUserVariable(c.ID().getText()));
            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionSKIPContext ) {
            SmallPearlParser.PositionSKIPContext c = (SmallPearlParser.PositionSKIPContext) ctx.position();
            ST e = m_group.getInstanceOf("get_statement_factor_position_skip");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", 1);
            }

            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionXContext ) {
            SmallPearlParser.PositionXContext c = (SmallPearlParser.PositionXContext) ctx.position();
            ST e = m_group.getInstanceOf("get_statement_factor_position_x");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", 1);
            }

            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionEOFContext ) {
            SmallPearlParser.PositionEOFContext c = (SmallPearlParser.PositionEOFContext) ctx.position();
            ST e = m_group.getInstanceOf("get_statement_factor_position_eof");
        }

        return st;
    }

    private ST getUserVariable(String user_variable) {
        ST st = m_group.getInstanceOf("user_variable");
        st.add("name", user_variable);
        return st;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // SendStatement ::=
    //   SEND [ Expression ] TO Name§Dation
    //    [ BY RST-S-CTRL-Format [ , RST-S-CTRL-Format ] ... ] ;
    ////////////////////////////////////////////////////////////////////////////////

    @Override
    public ST visitSendStatement(SmallPearlParser.SendStatementContext ctx) {
        ST st = m_group.getInstanceOf("send_statement");
        String dation = "";

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.SendToContext) {
                dation = ((SmallPearlParser.SendToContext)c).ID().getText();
            }
        }

        st.add("dation", getUserVariable(dation));

        for (int i = 0; i < ctx.take_send_rst_s_ctrl_format().size(); i++) {
            if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_RST(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_SContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_S(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_CONTROL(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            }
        }

        if ( ctx.expression() != null ) {
            ST el = m_group.getInstanceOf("send_expression");
            el.add("expression", getUserVariable((ctx.expression().getText())));
            el.add("dation", getUserVariable(dation));
            st.add("elements", el);
        }

        return st;
    }

    @Override
    public ST visitTakeStatement(SmallPearlParser.TakeStatementContext ctx) {
        ST st = m_group.getInstanceOf("take_statement");
        String dation = "";

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.TakeFromContext) {
                dation = ((SmallPearlParser.TakeFromContext)c).ID().getText();
            }
        }

        st.add("dation", getUserVariable(dation));

        for (int i = 0; i < ctx.take_send_rst_s_ctrl_format().size(); i++) {
            if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_RST(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_SContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_S(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_CONTROL(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            }
        }

        if ( ctx.ID() != null ) {
            ST el = m_group.getInstanceOf("take_id");
            el.add("id", getUserVariable(ctx.ID().getText()));
            el.add("dation", getUserVariable(dation));
            st.add("elements", el);
        }

        return st;
    }

    @Override
    public ST visitTake_send_rst_s_ctrl_format_RST(SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext ctx) {
        ST st = m_group.getInstanceOf("take_send_rst_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public ST visitTake_send_rst_s_ctrl_format_S(SmallPearlParser.Take_send_rst_s_ctrl_format_SContext ctx) {
        ST st = m_group.getInstanceOf("take_send_s_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public ST visitTake_send_rst_s_ctrl_format_CONTROL(SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext ctx) {
        ST st = m_group.getInstanceOf("take_send_control_position");

        if ( ctx.expression().size() == 3  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
            st.add("expression3", ctx.expression(2).getText());
        }
        else if ( ctx.expression().size() == 2  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
        }
        else if ( ctx.expression().size() == 1  ) {
            st.add("expression1", ctx.expression(0).getText());
        }

        return st;
    }

    @Override
    public ST visitReadStatement(SmallPearlParser.ReadStatementContext ctx) {
        ST st = m_group.getInstanceOf("read_statement");
        String dation = "";

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Read_fromContext) {
                dation = ((SmallPearlParser.Read_fromContext)c).ID().getText();
            }
        }

        st.add("dation", getUserVariable(dation));

        for (int i = 0; i < ctx.readPosition().size(); i++) {
            if ( ctx.readPosition(i) instanceof SmallPearlParser.ReadAbsPositionContext) {
                SmallPearlParser.ReadAbsPositionContext c = (SmallPearlParser.ReadAbsPositionContext) ctx.readPosition(i);
                if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionCOLContext ) {
                    ST el = visitReadWriteAbsolutePositionCOL((SmallPearlParser.ReadWriteAbsolutePositionCOLContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionLINEContext ) {
                    ST el = visitReadWriteAbsolutePositionLINE((SmallPearlParser.ReadWriteAbsolutePositionLINEContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionPOSContext ) {
                    ST el = visitReadWriteAbsolutePositionPOS((SmallPearlParser.ReadWriteAbsolutePositionPOSContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionSOPContext ) {
                    ST el = visitReadWriteAbsolutePositionSOP((SmallPearlParser.ReadWriteAbsolutePositionSOPContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
            }
            else if ( ctx.readPosition(i) instanceof SmallPearlParser.ReadRelPositionContext) {
                SmallPearlParser.ReadRelPositionContext c = (SmallPearlParser.ReadRelPositionContext) ctx.readPosition(i);
                if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionXContext ) {
                    ST el = visitReadWriteRelativePositionX((SmallPearlParser.ReadWriteRelativePositionXContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionSKIPContext ) {
                    ST el = visitReadWriteRelativePositionSKIP((SmallPearlParser.ReadWriteRelativePositionSKIPContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionPAGEContext ) {
                    ST el = visitReadWriteRelativePositionPAGE((SmallPearlParser.ReadWriteRelativePositionPAGEContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionADVContext ) {
                    ST el = visitReadWriteRelativePositionADV((SmallPearlParser.ReadWriteRelativePositionADVContext) c.readWriteRelativePosition());
                    el.add("dation", getUserVariable(dation));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionEOFContext ) {
                        ST el = visitReadWriteRelativePositionEOF((SmallPearlParser.ReadWriteRelativePositionEOFContext) c.readWriteRelativePosition());
                        el.add( "dation", getUserVariable(dation));
                        st.add("elements", el);
                }
            }
            else if ( ctx.readPosition(i) instanceof SmallPearlParser.ReadRSTPositionContext) {
                ST el = visitReadRSTPosition((SmallPearlParser.ReadRSTPositionContext) ctx.readPosition(i));
                el.add( "dation", getUserVariable(dation));
                st.add("elements", el);
            }
        }

        for (int i = 0; i < ctx.ID().size(); i++) {
            ST el = m_group.getInstanceOf("read_expression");
            el.add( "id", getUserVariable(ctx.ID(i).getText()));
            el.add( "dation", getUserVariable(dation));
            st.add( "elements", el);
        }

        return st;
    }

    @Override
    public ST visitWriteStatement(SmallPearlParser.WriteStatementContext ctx) {
        ST st = m_group.getInstanceOf("write_statement");
        st.add("dation", getUserVariable(ctx.ID().toString()));


        for (int i = 0; i < ctx.writePosition().size(); i++) {
            if ( ctx.writePosition(i) instanceof SmallPearlParser.WriteAbsPositionContext) {
                SmallPearlParser.WriteAbsPositionContext c = (SmallPearlParser.WriteAbsPositionContext) ctx.writePosition(i);
                if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionCOLContext ) {
                    ST el = visitReadWriteAbsolutePositionCOL((SmallPearlParser.ReadWriteAbsolutePositionCOLContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionLINEContext ) {
                    ST el = visitReadWriteAbsolutePositionLINE((SmallPearlParser.ReadWriteAbsolutePositionLINEContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionPOSContext ) {
                    ST el = visitReadWriteAbsolutePositionPOS((SmallPearlParser.ReadWriteAbsolutePositionPOSContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionSOPContext ) {
                    ST el = visitReadWriteAbsolutePositionSOP((SmallPearlParser.ReadWriteAbsolutePositionSOPContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
            }
            else if ( ctx.writePosition(i) instanceof SmallPearlParser.WriteRelPositionContext) {
                SmallPearlParser.WriteRelPositionContext c = (SmallPearlParser.WriteRelPositionContext) ctx.writePosition(i);
                if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionXContext ) {
                    ST el = visitReadWriteRelativePositionX((SmallPearlParser.ReadWriteRelativePositionXContext) c.readWriteRelativePosition());
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionSKIPContext ) {
                    ST el = visitReadWriteRelativePositionSKIP((SmallPearlParser.ReadWriteRelativePositionSKIPContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionPAGEContext ) {
                    ST el = visitReadWriteRelativePositionPAGE((SmallPearlParser.ReadWriteRelativePositionPAGEContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionADVContext ) {
                    ST el = visitReadWriteRelativePositionADV((SmallPearlParser.ReadWriteRelativePositionADVContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionEOFContext ) {
                    ST el = visitReadWriteRelativePositionEOF((SmallPearlParser.ReadWriteRelativePositionEOFContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
            }
            else if ( ctx.writePosition(i) instanceof SmallPearlParser.WriteRSTPositionContext) {
                ST el = visitWriteRSTPosition((SmallPearlParser.WriteRSTPositionContext) ctx.writePosition(i));
                el.add( "dation", getUserVariable(ctx.ID().toString()));
                st.add("elements", el);
            }
        }

        for (int i = 0; i < ctx.expression().size(); i++) {
            ST el = m_group.getInstanceOf("write_expression");
            el.add( "expression", getUserVariable(ctx.expression(i).getText()));
            el.add( "dation", getUserVariable(ctx.ID().toString()));
            st.add( "elements", el);
        }

        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionCOL( SmallPearlParser.ReadWriteAbsolutePositionCOLContext ctx) {
        ST st = m_group.getInstanceOf("read_write_col_position");
        st.add("expression", ctx.expression().getText());
        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionLINE( SmallPearlParser.ReadWriteAbsolutePositionLINEContext ctx) {
        ST st = m_group.getInstanceOf("read_write_line_position");
        st.add("expression", ctx.expression().getText());
        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionPOS( SmallPearlParser.ReadWriteAbsolutePositionPOSContext ctx) {
        ST st = m_group.getInstanceOf("read_write_pos_position");

        if ( ctx.expression().size() == 3  ) {
            st.add("expression1", getExpression(ctx.expression(0)));
            st.add("expression2", getExpression(ctx.expression(1)));
            st.add("expression3", getExpression(ctx.expression(2)));
        }
        else if ( ctx.expression().size() == 2  ) {
            st.add("expression1", getExpression(ctx.expression(0)));
            st.add("expression2", getExpression(ctx.expression(1)));
        }
        else if ( ctx.expression().size() == 1  ) {
            st.add("expression1", getExpression(ctx.expression(0)));
        }

        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionSOP( SmallPearlParser.ReadWriteAbsolutePositionSOPContext ctx) {
        ST st = m_group.getInstanceOf("read_write_sop_position");

        if ( ctx.ID().size() == 3  ) {
            st.add("id1", getUserVariable(ctx.ID(0).getText()));
            st.add("id2", getUserVariable(ctx.ID(1).getText()));
            st.add("id3", getUserVariable(ctx.ID(2).getText()));
        }
        else if ( ctx.ID().size() == 2  ) {
            st.add("id1", getUserVariable(ctx.ID(0).getText()));
            st.add("id2", getUserVariable(ctx.ID(1).getText()));
        }
        else if ( ctx.ID().size() == 1  ) {
            st.add("id1", getUserVariable(ctx.ID(0).getText()));
        }

        return st;
    }

    @Override
    public
    ST visitReadRSTPosition( SmallPearlParser.ReadRSTPositionContext ctx) {
        ST st = m_group.getInstanceOf("read_write_rst_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public
    ST visitWriteRSTPosition( SmallPearlParser.WriteRSTPositionContext ctx) {
        ST st = m_group.getInstanceOf("read_write_rst_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionX(SmallPearlParser.ReadWriteRelativePositionXContext ctx) {
        ST st = m_group.getInstanceOf("read_write_x_position");
        if ( ctx.expression() != null ) {
            st.add("expression", ctx.expression().getText());
        }
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionSKIP(SmallPearlParser.ReadWriteRelativePositionSKIPContext ctx) {
        ST st = m_group.getInstanceOf("read_write_skip_position");
        if ( ctx.expression() != null ) {
            st.add("expression", ctx.expression().getText());
        }
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionPAGE(SmallPearlParser.ReadWriteRelativePositionPAGEContext ctx) {
        ST st = m_group.getInstanceOf("read_write_page_position");
        if ( ctx.expression() != null ) {
            st.add("expression", ctx.expression().getText());
        }
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionADV(SmallPearlParser.ReadWriteRelativePositionADVContext ctx) {
        ST st = m_group.getInstanceOf("read_write_adv_position");

        if ( ctx.expression().size() == 3  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
            st.add("expression3", ctx.expression(2).getText());
        }
        else if ( ctx.expression().size() == 2  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
        }
        else if ( ctx.expression().size() == 1  ) {
            st.add("expression1", ctx.expression(0).getText());
        }

        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionEOF(SmallPearlParser.ReadWriteRelativePositionEOFContext ctx) {
        ST st = m_group.getInstanceOf("read_write_eof_position");
        return st;
    }

    @Override
    public ST visitCallStatement(SmallPearlParser.CallStatementContext ctx) {
        ST stmt = m_group.getInstanceOf("CallStatement");

        stmt.add("callee", ctx.ID());
        if ( ctx.listOfActualParameters() != null ) {
            stmt.add("ListOfActualParameters", visitListOfActualParameters(ctx.listOfActualParameters()));
        }

        return stmt;
    }

    @Override
    public ST visitListOfActualParameters(SmallPearlParser.ListOfActualParametersContext ctx) {
        ST stmt = m_group.getInstanceOf("ActualParameters");

        if ( ctx.expression() != null ) {
            for (int i = 0; i < ctx.expression().size(); i++) {
                ST param = m_group.getInstanceOf("ActualParameters");
                param.add("ActualParameter", getExpression(ctx.expression(i)));
                stmt.add("ActualParameter", param);
            }
        }

        return stmt;
    }

    private ST getActualParameters(List<SmallPearlParser.ExpressionContext> parameters) {
        ST stmt = m_group.getInstanceOf("ActualParameters");

        if ( parameters != null ) {
            for (int i = 0; i < parameters.size(); i++) {
                ST param = m_group.getInstanceOf("ActualParameters");
                param.add("ActualParameter", getExpression(parameters.get(i)));
                stmt.add("ActualParameter", param);
            }
        }

        return stmt;
    }

    private ST getIndices(List<SmallPearlParser.ExpressionContext> indices) {
        ST st = m_group.getInstanceOf("ArrayIndices");

        if ( indices != null ) {
            for (int i = 0; i < indices.size(); i++) {
                ST stIndex = m_group.getInstanceOf("ArrayIndex");
                stIndex.add("index", getExpression(indices.get(i)));
                st.add("indices", stIndex);
            }
        }

        return st;
    }


    @Override
    public ST visitCpp_inline(SmallPearlParser.Cpp_inlineContext ctx) {
        ST stmt = m_group.getInstanceOf("cpp_inline");

        stmt.add("body", "#warning __cpp__ inline inserted");

        int i;
        for (i = 0; i < ctx.CppStringLiteral().size(); i++) {
            String line = ctx.CppStringLiteral(i).toString();

            line = line.replaceAll("^\"", "");
            line = line.replaceAll("\"$", "");

            line = CommonUtils.unescapeCppString(line);

            stmt.add("body", line);
        }

        return stmt;
    }

    @Override
    public ST visitAtanExpression(SmallPearlParser.AtanExpressionContext ctx) {
        ST st = m_group.getInstanceOf("ATAN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitCosExpression(SmallPearlParser.CosExpressionContext ctx) {
        ST st = m_group.getInstanceOf("COS");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitExpExpression(SmallPearlParser.ExpExpressionContext ctx) {
        ST st = m_group.getInstanceOf("EXP");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitLnExpression(SmallPearlParser.LnExpressionContext ctx) {
        ST st = m_group.getInstanceOf("LN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitSinExpression(SmallPearlParser.SinExpressionContext ctx) {
        ST st = m_group.getInstanceOf("SIN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitSqrtExpression(SmallPearlParser.SqrtExpressionContext ctx) {
        ST st = m_group.getInstanceOf("SQRT");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitTanExpression(SmallPearlParser.TanExpressionContext ctx) {
        ST st = m_group.getInstanceOf("TAN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitTanhExpression(SmallPearlParser.TanhExpressionContext ctx) {
        ST st = m_group.getInstanceOf("TANH");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitAbsExpression(SmallPearlParser.AbsExpressionContext ctx) {
        ST st = m_group.getInstanceOf("ABS");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitSignExpression(SmallPearlParser.SignExpressionContext ctx) {
        ST st = m_group.getInstanceOf("SIGN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitRemainderExpression(SmallPearlParser.RemainderExpressionContext ctx) {
        ST st = m_group.getInstanceOf("REM");
        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));
        return st;
    }

    @Override
    public ST visitSizeofExpression(SmallPearlParser.SizeofExpressionContext ctx) {
        ST st = m_group.getInstanceOf("SIZEOF");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitEntierExpression(SmallPearlParser.EntierExpressionContext ctx) {
        ST st = m_group.getInstanceOf("ENTIER");
        if (m_debug) {
            System.out.println("CppCodeGeneratorVisitor: visitEntierExpression");
        }
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitRoundExpression(SmallPearlParser.RoundExpressionContext ctx) {
        ST st = m_group.getInstanceOf("ROUND");
        if (m_debug) {
            System.out.println("CppCodeGeneratorVisitor: visitRoundExpression");
        }

        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitEqRelationalExpression(SmallPearlParser.EqRelationalExpressionContext ctx) {
        ST st = m_group.getInstanceOf("EQ");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitNeRelationalExpression(SmallPearlParser.NeRelationalExpressionContext ctx) {
        ST st = m_group.getInstanceOf("NE");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitLtRelationalExpression(SmallPearlParser.LtRelationalExpressionContext ctx) {
        ST st = m_group.getInstanceOf("LT");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitLeRelationalExpression(SmallPearlParser.LeRelationalExpressionContext ctx) {
        ST st = m_group.getInstanceOf("LE");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitGtRelationalExpression(SmallPearlParser.GtRelationalExpressionContext ctx) {
        ST st = m_group.getInstanceOf("GT");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitGeRelationalExpression(SmallPearlParser.GeRelationalExpressionContext ctx) {
        ST st = m_group.getInstanceOf("GE");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitFitExpression(SmallPearlParser.FitExpressionContext ctx) {
        ST st = m_group.getInstanceOf("FIT");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitNowFunction(SmallPearlParser.NowFunctionContext ctx) {
        ST st = m_group.getInstanceOf("NOW");
        return st;
    }

    @Override
    public ST visitDationSpecification(SmallPearlParser.DationSpecificationContext ctx) {
        ST dationSpecifications = m_group.getInstanceOf("DationSpecifications");
        boolean hasGlobalAttribute = false;

        ArrayList<String> identifierDenotationList = null;
        if (ctx != null) {
            if (ctx.identifierDenotation() != null ){
                identifierDenotationList = getIdentifierDenotation(ctx.identifierDenotation());
            }

            if ( ctx.globalAttribute() != null ) {
                hasGlobalAttribute = true;
            }

            String dationClass = getDationClass(ctx.specifyTypeDation().classAttribute());

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                if ( dationClass.equals("SystemDationB")) {
                    ST specifyDation = m_group.getInstanceOf("SpecificationSystemDationB");
                    specifyDation.add( "name", identifierDenotationList.get(i));
                    dationSpecifications.add("decl", specifyDation);
                }
                else if ( dationClass.equals("SystemDationNB")) {
                    ST specifyDation = m_group.getInstanceOf("SpecificationSystemDationNB");
                    specifyDation.add( "name", identifierDenotationList.get(i));
                    dationSpecifications.add("decl", specifyDation);
                }
                else if ( dationClass.equals("DationTS")) {
                    ST specifyDation = m_group.getInstanceOf("SpecificationSystemDationTS");
                    specifyDation.add( "name", identifierDenotationList.get(i));
                    dationSpecifications.add("decl", specifyDation);

                }
                else if ( dationClass.equals("DationPG")) {
                    ST specifyDation = m_group.getInstanceOf("SpecificationSystemDationPG");
                    specifyDation.add( "name", identifierDenotationList.get(i));
                    dationSpecifications.add("decl", specifyDation);
                }
                else if ( dationClass.equals("DationRW")) {
                    ST specifyDation = m_group.getInstanceOf("SpecificationSystemDationRW");
                    specifyDation.add( "name", identifierDenotationList.get(i));
                    dationSpecifications.add("decl", specifyDation);

                }
            }
        }

        return dationSpecifications;
    }

    @Override
    public ST visitDationDeclaration(SmallPearlParser.DationDeclarationContext ctx) {
        ST dationDeclarations = m_group.getInstanceOf("DationDeclarations");
        ST typeDation = m_group.getInstanceOf("TypeDation");
        dationDeclarations.add("decl", visitIdentifierDenotation(ctx.identifierDenotation()));
        typeDation = getTypeDation(ctx.typeDation(),getDationClass(ctx.typeDation().classAttribute()));

        ST typology = m_group.getInstanceOf("Typology");
        ST accessAttributes = m_group.getInstanceOf("AccessAttributes");

        if (ctx.typology() != null) {
            typology = visitTypology(ctx.typology());
        }

        if (ctx.accessAttribute() != null) {
            accessAttributes = visitAccessAttribute(ctx.accessAttribute());
        }

        if (ctx.globalAttribute() != null) {
            visitGlobalAttribute(ctx.globalAttribute());
        }

        ArrayList<String> identifierDenotationList = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.IdentifierDenotationContext) {
                    identifierDenotationList = getIdentifierDenotation((SmallPearlParser.IdentifierDenotationContext) c);
                }
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                ST v = m_group.getInstanceOf("DationDeclaration");
                v.add("name", identifierDenotationList.get(i));
                v.add("TypeDation", typeDation);

                if (ctx.typology() != null) {
                    typology.add("name", identifierDenotationList.get(i));
                    v.add("Typology", typology);
                }

                v.add("Id", ctx.ID().getText());
                v.add("Dation", getDationClass(ctx.typeDation().classAttribute()));

                if (ctx.accessAttribute() != null) {
                    typeDation.add("AccessAttribute", accessAttributes);
                }

                if (ctx.typology() != null) {
                    typeDation.add("Dim", identifierDenotationList.get(i));
                }

                dationDeclarations.add("decl", v);
            }
        }

        return dationDeclarations;
    }

    @Override
    public ST visitTypeDation(SmallPearlParser.TypeDationContext ctx) {
        ST st = m_group.getInstanceOf("TypeDation");
        ST sourceSinkAttributte = m_group.getInstanceOf("SourceSinkAttribute");
        sourceSinkAttributte.add("attribute",ctx.sourceSinkAttribute().getText());
        st.add("SourceSinkAttribute", sourceSinkAttributte);

        if ( ctx.classAttribute() != null ) {
            st.add( "ClassAttribute", getClassAttribute(ctx.classAttribute()));
        }

        return st;
    }

    private ST getTypeDation(SmallPearlParser.TypeDationContext ctx, String dationClass) {
        ST st = m_group.getInstanceOf("TypeDation");
        ST sourceSinkAttributte = m_group.getInstanceOf("SourceSinkAttribute");
        sourceSinkAttributte.add("attribute",ctx.sourceSinkAttribute().getText());
        st.add("SourceSinkAttribute", sourceSinkAttributte);

        if (dationClass.equals("DationRW") && ctx.classAttribute() != null ) {
            st.add( "ClassAttribute", getClassAttribute(ctx.classAttribute()));
        }

        return st;
    }

    @Override
    public ST visitSpecifyTypeDation(SmallPearlParser.SpecifyTypeDationContext ctx) {
        if ( ctx.classAttribute() != null ) {
            if ( ctx.classAttribute().systemDation() != null ) {
            }

            if ( ctx.classAttribute().alphicDation() != null ) {
            }
            else if ( ctx.classAttribute().basicDation() != null ) {
                ST st = m_group.getInstanceOf("DationSpecificationBasic");
                st.add("SourceSinkAttribute", ctx.sourceSinkAttribute().getText());
                return st;
            }
        }

        return null;
    }

    @Override
    public ST visitTypology(SmallPearlParser.TypologyContext ctx) {
        ST st = m_group.getInstanceOf("Typology");
        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Dimension1StarContext) {
                st.add("DIM1", -1);
                st.add("DIM1Unlimited", 1);
            } else if (c instanceof SmallPearlParser.Dimension1IntegerContext) {
                st.add("DIM1", Integer.valueOf(((SmallPearlParser.Dimension1IntegerContext) c).IntegerConstant().toString()));
            } else if (c instanceof SmallPearlParser.Dimension2IntegerContext) {
                st.add("DIM2", Integer.valueOf(((SmallPearlParser.Dimension2IntegerContext) c).IntegerConstant().toString()));
            } else if (c instanceof SmallPearlParser.Dimension3IntegerContext) {
                st.add("DIM3", Integer.valueOf(((SmallPearlParser.Dimension3IntegerContext) c).IntegerConstant().toString()));
            }
        }

        return st;
    }


    // OpenPEARL Language Report: 11.5
    //
    //        | BASIC         | ALPHIC         | ALL / type
    // -------+---------------+----------------+----------------
    // SYSTEM | SystemDationB | SystemDationNB | SystemDationNB
    //        | DationTS      | DationPG       | DationRW
    // -------+---------------+----------------+----------------

    private String getDationClass(SmallPearlParser.ClassAttributeContext ctx) throws InternalCompilerErrorException {
        if (ctx.systemDation() != null) {
            if (ctx.basicDation() != null) {
                return "SystemDationB";
            }

            if (ctx.alphicDation() != null) {
                return "SystemDationNB";
            }

            return "SystemDationNB";
        }

        if (ctx.basicDation() != null) {
            return "DationTS";
        }

        if (ctx.alphicDation() != null) {
            return "DationPG";
        }

        if (ctx.typeOfTransmissionData() != null ) {
            if (ctx.typeOfTransmissionData() instanceof SmallPearlParser.TypeOfTransmissionDataALLContext) {
                return "DationRW";
            }

            if (ctx.typeOfTransmissionData() instanceof SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext) {
                return "DationRW";
            }
        }

        throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    private ST getStepSize(SmallPearlParser.ClassAttributeContext ctx) {
        ST st = m_group.getInstanceOf("StepSize");

        st.add("type", "Fixed");
        st.add("size", "31");

        return st;
    }

    private ST getClassAttribute(SmallPearlParser.ClassAttributeContext ctx) {
        ST st = m_group.getInstanceOf("ClassAttribute");

        if (ctx.systemDation() != null) {
            st.add("system", "1");
        }

        if (ctx.alphicDation() != null) {
            st.add("alphic", "1");
        } else if (ctx.basicDation() != null) {
            st.add("basic", "1");
        }

        if (ctx.typeOfTransmissionData() != null) {
            st.add("attribute", getTypeOfTransmissionData((SmallPearlParser.TypeOfTransmissionDataContext) ctx.typeOfTransmissionData()));
        }

        return st;
    }


    private ST getTypeOfTransmissionData(SmallPearlParser.TypeOfTransmissionDataContext ctx) {
        ST st = m_group.getInstanceOf("TypeOfTransmissionData");

        if (ctx instanceof SmallPearlParser.TypeOfTransmissionDataALLContext) {
            st.add("all", "1");
        } else if (ctx instanceof SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext) {
            SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext c = (SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext) ctx;
            st.add("type", visitSimpleType(c.simpleType()));
        }

        return st;
    }

    private ST getAccessAttribute(SmallPearlParser.AccessAttributeContext ctx) {
        ST st = m_group.getInstanceOf("AccessAttribute");

        for (ParseTree c : ctx.children) {
            st.add("attribute", c.getText());
        }

        return st;
    }

    @Override
    public ST visitAccessAttribute(SmallPearlParser.AccessAttributeContext ctx) {
        ST st = m_group.getInstanceOf("AccessAttribute");

        for (ParseTree c : ctx.children) {
            st.add("attribute", c.getText());
        }

        return st;
    }

    @Override
    public ST visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        ST st = m_group.getInstanceOf("block_statement");

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);

        for (ParseTree c : ctx.children) {
            if ( c instanceof SmallPearlParser.ScalarVariableDeclarationContext ) {
                st.add("code", visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext)c));
            }
            else if ( c instanceof SmallPearlParser.ArrayVariableDeclarationContext ) {
                st.add("code", visitArrayVariableDeclaration((SmallPearlParser.ArrayVariableDeclarationContext)c));
            }
            else if ( c instanceof SmallPearlParser.StatementContext ) {
                st.add("code", visitStatement((SmallPearlParser.StatementContext)c));
            }
        }

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();

        return st;
    }

    @Override
    public ST visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        ST st = m_group.getInstanceOf("LoopStatement");
        TypeDefinition fromType = null;
        TypeDefinition byType = null;
        TypeDefinition toType = null;
        Integer rangePrecision = 31;
        Boolean loopCounterNeeded = false;

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);

        st.add("srcLine", ctx.start.getLine());

        if ( ctx.loopStatement_for() != null) {
            st.add( "variable", ctx.loopStatement_for().ID().toString());
            loopCounterNeeded = true;
        }

        if ( ctx.loopStatement_from() != null) {
            boolean old_map_to_const = m_map_to_const;

            fromType = m_expressionTypeVisitor.lookupType(ctx.loopStatement_from().expression());

            m_map_to_const = false;
            st.add("from", getExpression(ctx.loopStatement_from().expression()));
            m_map_to_const = old_map_to_const;
        }

        if ( ctx.loopStatement_to() != null) {
            boolean old_map_to_const = m_map_to_const;

            toType = m_expressionTypeVisitor.lookupType(ctx.loopStatement_to().expression());

            m_map_to_const = false;
            st.add( "to", getExpression(ctx.loopStatement_to().expression()));
            m_map_to_const = old_map_to_const;

            loopCounterNeeded = true;
        }

        if ( fromType != null && toType != null) {
            rangePrecision = Math.max(((TypeFixed)fromType).getPrecision(), ((TypeFixed)toType).getPrecision());
            st.add("fromPrecision",rangePrecision);
            st.add("toPrecision",rangePrecision);
            loopCounterNeeded = true;
        }
        else if ( fromType != null && toType == null) {
            rangePrecision = ((TypeFixed)fromType).getPrecision();
            st.add("fromPrecision",((TypeFixed)fromType).getPrecision());
        }
        else if ( fromType == null && toType != null) {
            rangePrecision = ((TypeFixed)toType).getPrecision();
            st.add("toPrecision",((TypeFixed)toType).getPrecision());
            loopCounterNeeded = true;
        }

        st.add("rangePrecision",rangePrecision);

        if ( ctx.loopStatement_by() != null) {
            boolean old_map_to_const = m_map_to_const;

            byType = m_expressionTypeVisitor.lookupType(ctx.loopStatement_by().expression());

            m_map_to_const = false;
            st.add("by", getExpression(ctx.loopStatement_by().expression()));
            st.add("byPrecision", rangePrecision);
            m_map_to_const = old_map_to_const;

            loopCounterNeeded = true;
        }

        if ( ctx.loopStatement_while() != null && ctx.loopStatement_while().expression() != null) {
            ST wc = getExpression(ctx.loopStatement_while().expression());
            String s = wc.toString();
            if ( wc.toString().length() > 0 ) {
                ST cast = m_group.getInstanceOf("CastBitToBoolean");
                cast.add("name", wc);
                st.add("while_cond", cast);
            }
        }

        for (int i = 0; i < ctx.scalarVariableDeclaration().size(); i++) {
            st.add("body", visitScalarVariableDeclaration(ctx.scalarVariableDeclaration(i)));
        }

        for (int i = 0; i < ctx.arrayVariableDeclaration().size(); i++) {
            st.add("body", visitArrayVariableDeclaration(ctx.arrayVariableDeclaration(i)));
        }

        for ( int i = 0; i < ctx.statement().size(); i++) {
            st.add("body", visitStatement(ctx.statement(i)));
        }

        if ( ctx.loopStatement_end().ID() != null) {
            st.add( "label_end", ctx.loopStatement_end().ID().toString());
        }

        if ((ctx.loopStatement_to() != null) || (ctx.loopStatement_for() != null) || (ctx.loopStatement_by() != null)) {
            st.add( "countLoopPass", 1);
        }

        if (loopCounterNeeded) {
            st.add( "GenerateLoopCounter", 1);
        }

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();

        return st;
    }

    @Override
    public ST visitExitStatement(SmallPearlParser.ExitStatementContext ctx) {
        ST st = m_group.getInstanceOf("ExitStatement");

        if ( ctx.ID() != null) {
            st.add("label", ctx.ID().toString());
        }

        return st;
    }

    @Override
    public ST visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        ST st = m_group.getInstanceOf("ProcedureDeclaration");
        st.add("id", ctx.ID().getText());

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.ProcedureBodyContext) {
                st.add("body", visitProcedureBody((SmallPearlParser.ProcedureBodyContext) c));
            } else
            if (c instanceof SmallPearlParser.ResultAttributeContext) {
                st.add("resultAttribute", visitResultAttribute((SmallPearlParser.ResultAttributeContext) c));
            } else
            if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                st.add("globalAttribute", visitGlobalAttribute((SmallPearlParser.GlobalAttributeContext) c));
            } else
            if (c instanceof SmallPearlParser.ListOfFormalParametersContext) {
                st.add("listOfFormalParameters", visitListOfFormalParameters((SmallPearlParser.ListOfFormalParametersContext) c));
            }
        }

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return st;
    }

    private ST getProcedureSpecification(SmallPearlParser.ProcedureDeclarationContext ctx) {
        ST st = m_group.getInstanceOf("ProcedureSpecification");

        st.add("id", ctx.ID().getText());

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.ResultAttributeContext) {
                st.add("resultAttribute", visitResultAttribute((SmallPearlParser.ResultAttributeContext) c));
            } else
            if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                st.add("globalAttribute", visitGlobalAttribute((SmallPearlParser.GlobalAttributeContext) c));
            } else
            if (c instanceof SmallPearlParser.ListOfFormalParametersContext) {
                st.add("listOfFormalParameters", visitListOfFormalParameters((SmallPearlParser.ListOfFormalParametersContext) c));
            }
        }

        return st;
    }

    @Override
    public ST visitListOfFormalParameters(SmallPearlParser.ListOfFormalParametersContext ctx) {
        ST st = m_group.getInstanceOf("ListOfFormalParameters");

        if (ctx != null) {
            for (int i = 0; i < ctx.formalParameter().size(); i++) {
                st.add("FormalParameters", visitFormalParameter(ctx.formalParameter(i)));
            }
        }

        return st;
    }

    @Override
    public ST visitFormalParameter(SmallPearlParser.FormalParameterContext ctx) {
        ST st = m_group.getInstanceOf("FormalParameters");

        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                ST param = m_group.getInstanceOf("FormalParameter");
                param.add("id", ctx.ID(i));
                param.add("type", visitParameterType(ctx.parameterType()));

                if ( ctx.assignmentProtection() != null ) {
                    param.add("assignmentProtection", "");
                }

                if ( ctx.passIdentical() != null ) {
                    param.add("passIdentical", "");
                }

                st.add( "FormalParameter", param);
            }
        }

        return st;
    }

    @Override
    public ST visitParameterType(SmallPearlParser.ParameterTypeContext ctx) {
        ST st = m_group.getInstanceOf("ParameterType");

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.SimpleTypeContext) {
                st.add("type", visitSimpleType(ctx.simpleType()));
            }
            else if (c instanceof SmallPearlParser.TypeReferenceContext) {
                st.add("type", visitTypeReference(ctx.typeReference()));
            }
        }

        return st;
    }

    @Override
    public ST visitProcedureBody(SmallPearlParser.ProcedureBodyContext ctx) {
        ST st = m_group.getInstanceOf("ProcedureBody");

        if(ctx != null && ctx.children != null ) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ScalarVariableDeclarationContext) {
                    st.add("declarations", visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.ArrayVariableDeclarationContext) {
                    st.add("declarations", visitArrayVariableDeclaration((SmallPearlParser.ArrayVariableDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.StatementContext) {
                    st.add("statements", visitStatement((SmallPearlParser.StatementContext) c));
                } else if (c instanceof SmallPearlParser.DationDeclarationContext) {
                    new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
        }

        return st;
    }

    @Override
    public ST visitGlobalAttribute(SmallPearlParser.GlobalAttributeContext ctx) {
        ST st = m_group.getInstanceOf("GlobalAttribute");

        st.add("id", ctx.ID().getText());

        return st;
    }

    @Override
    public ST visitResultAttribute(SmallPearlParser.ResultAttributeContext ctx) {
        ST st = m_group.getInstanceOf("ResultAttribute");
        st.add("resultType", visitResultType(ctx.resultType()));
        return st;
    }

    @Override
    public ST visitResultType(SmallPearlParser.ResultTypeContext ctx) {
        ST st = m_group.getInstanceOf("ResultType");

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.SimpleTypeContext) {
                st.add("type", visitSimpleType(ctx.simpleType()));
            }
            else if (c instanceof SmallPearlParser.TypeReferenceContext) {
                st.add("type", visitTypeReference(ctx.typeReference()));
            }
        }

        return st;
    }

    @Override
    public ST visitUsername_declaration(SmallPearlParser.Username_declarationContext ctx) {
        return visitChildren(ctx);
    }

    @Override
    public ST visitUsername_declaration_without_data_flow_direction(SmallPearlParser.Username_declaration_without_data_flow_directionContext ctx) {
        return visitChildren(ctx);
    }

    @Override
    public ST visitUsername_declaration_with_data_flow_direction(SmallPearlParser.Username_declaration_with_data_flow_directionContext ctx) {
        return visitChildren(ctx);
    }

    @Override
    public ST visitUserConfigurationWithAssociation(SmallPearlParser.UserConfigurationWithAssociationContext ctx) {
        return visitChildren(ctx);
    }

    @Override
    public ST visitUserConfigurationWithoutAssociation(SmallPearlParser.UserConfigurationWithoutAssociationContext ctx) {
        return visitChildren(ctx);
    }


    @Override
    public ST visitUsername_parameters(SmallPearlParser.Username_parametersContext ctx) {
        return visitChildren(ctx);
    }


    @Override
    public ST visitTOFIXEDExpression(SmallPearlParser.TOFIXEDExpressionContext ctx) {
        TypeDefinition op = m_expressionTypeVisitor.lookupType(ctx.expression());

        if ( op instanceof TypeBit) {
            ST st = m_group.getInstanceOf("BITSTOFIXED");
            st.add("operand", getExpression(ctx.expression()));
            return st;
        } else if ( op instanceof TypeChar ) {
            ST st = m_group.getInstanceOf("CHARACTERSTOFIXED");
            st.add("operand", getExpression(ctx.expression()));
            return st;

        }

        return null;
    }

    @Override
    public ST visitTOFLOATExpression(SmallPearlParser.TOFLOATExpressionContext ctx) {
        TypeDefinition op = m_expressionTypeVisitor.lookupType(ctx.expression());

        ST st = m_group.getInstanceOf("TOFLOAT");
        st.add("operand",  getExpression(ctx.expression()));

        if ( op instanceof TypeFixed) {
            st.add("precision", ((TypeFixed)op).getPrecision());
        }
        else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return st;
    }

        @Override
    public ST visitTOBITExpression(SmallPearlParser.TOBITExpressionContext ctx) {
        TypeDefinition op = m_expressionTypeVisitor.lookupType(ctx.expression());

        ST st = m_group.getInstanceOf("TOBIT");
        st.add("operand",  getExpression(ctx.expression()));

        if ( op instanceof TypeFixed) {
            st.add("noOfBits", ((TypeFixed)op).getPrecision() + 1);
        }
        else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return st;
    }

    @Override
    public ST visitCONTExpression(SmallPearlParser.CONTExpressionContext ctx) {
        TypeDefinition op = m_expressionTypeVisitor.lookupType(ctx.expression());

        ST st = m_group.getInstanceOf("CONT");
        st.add("operand",  getExpression(ctx.expression()));

        return st;
    }

    @Override
    public ST visitArrayVariableDeclaration(SmallPearlParser.ArrayVariableDeclarationContext ctx) {
        ST declarations = m_group.getInstanceOf("ArrayVariableDeclarations");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitArrayVariableDeclaration");
        }

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ArrayDenotationContext) {
                    declarations.add("declarations", visitArrayDenotation((SmallPearlParser.ArrayDenotationContext) c));
                }
            }
        }

        return declarations;
    }

    @Override
    public ST visitArrayDenotation(SmallPearlParser.ArrayDenotationContext ctx) {
        ST declarations = m_group.getInstanceOf("ArrayVariableDeclarations");

        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.ID().get(i).toString());

            if ( entry == null || !(entry instanceof VariableEntry)) {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            VariableEntry variableEntry = (VariableEntry)entry;

            if ( variableEntry.getType() instanceof TypeArray) {
                ArrayList<ST> initElementList = null;

                ST declaration = m_group.getInstanceOf("ArrayVariableDeclaration");

                declaration.add("name", variableEntry.getName());

                if ( variableEntry.getType() instanceof TypeArray) {
                    TypeArray type = (TypeArray)variableEntry.getType();
                    declaration.add("type", type.getBaseType().toST(m_group));
                }
                else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                declaration.add("assignmentProtection", variableEntry.getAssigmentProtection());
                declaration.add("totalNoOfElements", ((TypeArray)variableEntry.getType()).getTotalNoOfElements());

                if (ctx.arrayInitialisationAttribute() != null) {
                    declaration.add("initElements", getArrayInitialisationAttribute(ctx.arrayInitialisationAttribute(),
                                                                                    ((TypeArray)variableEntry.getType()).getTotalNoOfElements()));
                }

                declarations.add("declarations",declaration);
            }
        }

        return declarations;
    }


    @Override
    public ST visitConvertStatement(SmallPearlParser.ConvertStatementContext ctx) {
        ST st = m_group.getInstanceOf("ConvertStatement");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitConvertStatement");
        }

        if ( ctx.convertToStatement() != null ) {
            st.add("convert", visitConvertToStatement(ctx.convertToStatement()));
        }
        else if ( ctx.convertFromStatement() != null ) {
            st.add("convert", visitConvertFromStatement(ctx.convertFromStatement()));
        }

        return st;
    }


    @Override
    public ST visitFormatOrPositionConvert(SmallPearlParser.FormatOrPositionConvertContext ctx) {
        System.out.println("CppCodeGeneratorVisitor: visitFormatOrPositionConvert");
        visitChildren(ctx);
        return null;
    }

    @Override
    public ST visitFormatFactorConvert(SmallPearlParser.FormatFactorConvertContext ctx) {
        System.out.println("CppCodeGeneratorVisitor: visitFormatFactorConvert");
        visitChildren(ctx);
        return null;

    }

    @Override
    public ST visitFormatConvert(SmallPearlParser.FormatConvertContext ctx) {
        System.out.println("CppCodeGeneratorVisitor: visitFormatConvert");
        visitChildren(ctx);
        return null;

    }

//    @Override
//    public ST visitPositionConvert(SmallPearlParser.PositionConvertContext ctx) {
//        System.out.println("CppCodeGeneratorVisitor: visitPositionConvert");
//        visitChildren(ctx);
//        return null;
//
//    }


    @Override
    public ST visitConvertToStatement(SmallPearlParser.ConvertToStatementContext ctx) {
        ST st = m_group.getInstanceOf("ConvertToStatement");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitConvertToStatement");
        }

        visitChildren(ctx);

        int j = 0;

        int formatCount = 0;
        for ( int i = 0; i < ctx.formatOrPositionConvert().size(); i++ ) {
            if (ctx.formatOrPositionConvert(i) instanceof SmallPearlParser.FormatOrPositionConvertContext) {
                formatCount++;
            }
        }

        if ( formatCount > ctx.expression().size() ) {
            throw new NoOfFormatSpecifiersDoesNotMatchException("put_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return st;
    }

    @Override
    public ST visitConvertFromStatement(SmallPearlParser.ConvertFromStatementContext ctx) {
        ST st = m_group.getInstanceOf("ConvertFromStatement");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitConvertFromStatement");
        }

        return st;
    }

    @Override
    public ST visitCase1CharSlice(SmallPearlParser.Case1CharSliceContext ctx) {
        ST st = m_group.getInstanceOf("StringSlice");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitCase1CharSlice");
        }

        st.add("lwb", getExpression(ctx.expression()).render());
        st.add("upb", getExpression(ctx.expression()).render());
        st.add("id", ctx.ID().getText());
        st.add("size",1);

        return st;
    }

    @Override
    public ST visitCase2CharSlice(SmallPearlParser.Case2CharSliceContext ctx) {
        ST st = m_group.getInstanceOf("StringSlice");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitCase2CharSlice");
        }

        st.add("lwb", getExpression(ctx.expression(0)).render());
        st.add("upb", getExpression(ctx.expression(1)).render());
        st.add("id", ctx.ID().getText());
        st.add("size",1);

        return st;
    }

    @Override
    public ST visitCase3CharSlice(SmallPearlParser.Case3CharSliceContext ctx) {
        ST st = m_group.getInstanceOf("StringSlice");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitCase3CharSlice");
        }

        throw new NotYetImplementedException("Char Slice Case#3", 0, 0);

//        return st;
    }

    @Override
    public ST visitCase4CharSlice(SmallPearlParser.Case4CharSliceContext ctx) {
        ST st = m_group.getInstanceOf("StringSlice");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitCase4CharSlice");
        }

        throw new NotYetImplementedException("Char Slice Case4", 0, 0);

        // return st;
    }

    @Override
    public ST visitCase1BitSlice(SmallPearlParser.Case1BitSliceContext ctx) {
        ST st = m_group.getInstanceOf("BitSliceRHS");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitCase1BitSlice");
        }

        ConstantValue offset = m_constantExpressionEvaluatorVisitor.lookup(ctx.constantFixedExpression());

        if ( offset == null || !(offset instanceof ConstantFixedValue)) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        st.add("id", ctx.ID().getText());
        st.add("lwb", offset.toString());
        st.add("upb", offset.toString());
        st.add("size", 1);

        return st;
    }

    @Override
    public ST visitCase2BitSlice(SmallPearlParser.Case2BitSliceContext ctx) {
        ST st = m_group.getInstanceOf("BitSliceRHS");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitCase2BitSlice");
        }

        if ( ctx.constantFixedExpression().size() != 2) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        ConstantValue lwb = m_constantExpressionEvaluatorVisitor.lookup(ctx.constantFixedExpression(0));
        ConstantValue upb = m_constantExpressionEvaluatorVisitor.lookup(ctx.constantFixedExpression(1));

        if ( lwb == null || upb ==null || !(lwb instanceof ConstantFixedValue) || !(upb instanceof ConstantFixedValue)) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        long size = ((ConstantFixedValue) upb).getValue() - ((ConstantFixedValue) lwb).getValue() + 1;

        st.add("id", ctx.ID().getText());
        st.add("lwb", lwb.toString());
        st.add("upb", upb.toString());
        st.add("size", size);

        return st;
    }


    @Override
    public ST visitInterruptSpecification( SmallPearlParser.InterruptSpecificationContext ctx) {
        ST st = m_group.getInstanceOf("InterruptSpecifications");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitInterruptSpecification");
        }

        for ( int i = 0; i < ctx.ID().size(); i++)
        {
            ST spec = m_group.getInstanceOf("InterruptSpecification");
            spec.add( "id", ctx.ID(i));
            st.add("specs", spec);
        }
        return st;
    }

    @Override
    public ST visitInterrupt_statement( SmallPearlParser.Interrupt_statementContext ctx) {
        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitInterrupt_statement");
        }

        return visitChildren(ctx);
    }
    ;
    @Override
    public ST visitEnableStatement( SmallPearlParser.EnableStatementContext ctx) {
        ST st = m_group.getInstanceOf("EnableStatement");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitEnableStatement");
        }

        st.add("id",ctx.ID());
        return st;
    }

    @Override
    public ST visitDisableStatement( SmallPearlParser.DisableStatementContext ctx) {
        ST st = m_group.getInstanceOf("DisableStatement");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitDisableStatement");
        }

        st.add("id",ctx.ID());
        return st;
    }

    @Override
    public ST visitTriggerStatement( SmallPearlParser.TriggerStatementContext ctx) {
        ST st = m_group.getInstanceOf("TriggerStatement");

        if (m_verbose > 0) {
            System.out.println("CppCodeGeneratorVisitor: visitTriggerStatement");
        }

        st.add("id",ctx.ID());
        return st;
    }

}
