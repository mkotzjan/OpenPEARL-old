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

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.antlr.v4.semantics.SymbolChecks;
import org.smallpearl.compiler.SymbolTable.*;
import org.stringtemplate.v4.ST;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Collections;


public class SymbolTableVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;

    public SymbolTable symbolTable;
    private SymbolTableEntry m_currentEntry;
    private SymbolTable m_currentSymbolTable;
    private LinkedList<LinkedList<SemaphoreEntry>> m_listOfTemporarySemaphoreArrays;
    private TypeDefinition m_type;
    private ParseTreeProperty<SymbolTable> m_symboltablePerContext = null;

    public SymbolTableVisitor(int verbose) {

        m_verbose = verbose;

        if (m_verbose > 0) {
            System.out.println("Building new symbol table");
        }

        this.symbolTable = new org.smallpearl.compiler.SymbolTable.SymbolTable();
        this.m_listOfTemporarySemaphoreArrays = new LinkedList<LinkedList<SemaphoreEntry>>();
        this.m_symboltablePerContext =  new ParseTreeProperty<SymbolTable>();
    }

    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitModule");
        }

        org.smallpearl.compiler.SymbolTable.ModuleEntry moduleEntry = new org.smallpearl.compiler.SymbolTable.ModuleEntry(ctx.ID().getText(), ctx, null);
        this.m_currentSymbolTable = this.symbolTable.newLevel(moduleEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable );

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
                }
            }
        }

        return null;
    }

    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        Boolean isMain = false;
        Boolean isGlobal = false;
        Integer priority = -1;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: vistTaskDeclaration");
        }

        isMain = ctx.task_main() != null;
        if (ctx.priority() != null) {
            priority = Integer.parseInt(ctx.priority().IntegerConstant().getText());
        }

        TaskEntry taskEntry = new TaskEntry(ctx.ID().getText(), priority, isMain, isGlobal, ctx, this.m_currentSymbolTable);
        this.m_currentSymbolTable = this.m_currentSymbolTable.newLevel(taskEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable );

        visitChildren(ctx);

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        String globalId = null;
        LinkedList<FormalParameter> formalParameters = null;
        TypeDefinition resultType = null;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitProcedureDeclaration");
        }

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.ResultAttributeContext) {
                resultType = getResultAttribute((SmallPearlParser.ResultAttributeContext)c);
            } else
            if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                SmallPearlParser.GlobalAttributeContext globalCtx = (SmallPearlParser.GlobalAttributeContext) c;
                globalId = ctx.ID().getText();
            } else
            if (c instanceof SmallPearlParser.ListOfFormalParametersContext) {
                SmallPearlParser.ListOfFormalParametersContext listOfFormalParametersContext = (SmallPearlParser.ListOfFormalParametersContext)c;
                formalParameters = getListOfFormalParameters((SmallPearlParser.ListOfFormalParametersContext) c);
            }
        }

        ProcedureEntry procedureEntry = new ProcedureEntry(ctx.ID().getText(), formalParameters, resultType, globalId, ctx, this.m_currentSymbolTable);
        this.m_currentSymbolTable = this.m_currentSymbolTable.newLevel(procedureEntry);

        /* Enter formal parameter into the local symboltable of this procedure */
        if ( formalParameters != null && formalParameters.size() > 0 ) {
            for (FormalParameter formalParameter : formalParameters) {
                VariableEntry param = new VariableEntry(formalParameter.name, formalParameter.type, formalParameter.assignmentProtection, null);
                this.m_currentSymbolTable.enter(param);
            }
        }

        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable );

        visitChildren(ctx);

        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    private TypeDefinition getResultAttribute(SmallPearlParser.ResultAttributeContext ctx) {
        for (ParseTree c : ctx.resultType().children) {
            if (c instanceof SmallPearlParser.SimpleTypeContext) {
                visitSimpleType((SmallPearlParser.SimpleTypeContext)c);
                return m_type;
            }
        }

        return null;
    }

    private  LinkedList<FormalParameter> getListOfFormalParameters(SmallPearlParser.ListOfFormalParametersContext ctx) {
        LinkedList<FormalParameter> listOfFormalParameters = new  LinkedList<FormalParameter>();

        if (ctx != null) {
            for (int i = 0; i < ctx.formalParameter().size(); i++) {
                getFormalParameter(listOfFormalParameters , ctx.formalParameter(i));
            }
        }

        return listOfFormalParameters;
    }

    private Void getFormalParameter(LinkedList<FormalParameter> listOfFormalParameters , SmallPearlParser.FormalParameterContext ctx) {
        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                FormalParameter formalParameter = null;
                String name = null;
                Boolean assignmentProtection = false;
                Boolean passIdentical = false;

                name = ctx.ID(i).getText();

                if ( ctx.assignmentProtection() != null ) {
                    assignmentProtection = true;
                }

                if ( ctx.passIdentical() != null ) {
                    passIdentical = true;
                }

                getParameterType(ctx.parameterType());
                listOfFormalParameters.add(new FormalParameter(name,m_type,assignmentProtection));
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

        if ( ctx.ID() != null ) {
            blockLabel = ctx.ID().toString();
        }
        else {
            blockLabel = "?anonymous?";
        }

        BlockEntry blockEntry = new BlockEntry(blockLabel, ctx, m_currentSymbolTable);

        m_currentSymbolTable = m_currentSymbolTable.newLevel(blockEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable );

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
        ArrayList<ST> initElementList = null;

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
                }
            }

            if (initElementList != null && identifierDenotationList.size() != initElementList.size()) {
                throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                VariableEntry variableEntry = new VariableEntry(identifierDenotationList.get(i), m_type, ctx);
                if (!m_currentSymbolTable.enter(variableEntry)) {
                    System.out.println("ERR: Double definition of " + identifierDenotationList.get(i));
                }
            }
        }

        return null;
    }

    @Override
    public Void visitTypeAttribute(SmallPearlParser.TypeAttributeContext ctx) {
        if (ctx.simpleType() != null) {
            visitSimpleType(ctx.simpleType());
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
    public Void visitTypeInteger(SmallPearlParser.TypeIntegerContext ctx) {
        Integer size = 31;

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

        Integer length = 1;

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
        Integer size = 1;

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
        Integer precision = 53;


        if (ctx.IntegerConstant() != null) {
            precision = Integer.parseInt(ctx.IntegerConstant().getText());

            if (precision != 24 && precision != 53) {
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
                for (int j = 0; j < semaphores.size(); j++) {
                    if ( semaphores.get(j).compareTo(listOfSemaphores.get(j)) == 0) {
                        found = true;
                        break;
                    }
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

    @Override
    public Void visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        String blockLabel = null;

        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitLoopStatement");
        }

        for (int i = 0; i < ctx.scalarVariableDeclaration().size(); i++) {
            visitScalarVariableDeclaration(ctx.scalarVariableDeclaration(i));
        }

        for ( int i = 0; i < ctx.statement().size(); i++) {
        }

        if ( ctx.loopStatement_end().ID() != null) {
            blockLabel = ctx.loopStatement_end().ID().getText();
        }

        LoopEntry loopEntry = new LoopEntry(blockLabel, ctx, m_currentSymbolTable);

        m_currentSymbolTable = m_currentSymbolTable.newLevel(loopEntry);
        this.m_symboltablePerContext.put(ctx, this.m_currentSymbolTable );

        if ( ctx.loopStatement_for() != null) {
            VariableEntry controlVariable = new VariableEntry(ctx.loopStatement_for().ID().getText(), new TypeFixed(31), true, null);
            m_currentSymbolTable.enter(controlVariable);
        }

        visitChildren(ctx);
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

    public SymbolTable getSymbolTablePerContext(ParseTree ctx) {
        return m_symboltablePerContext.get(ctx);
    }
}
