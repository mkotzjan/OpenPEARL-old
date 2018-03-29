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

package org.smallpearl.compiler.SemanticAnalysis;

import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ParseTree;
import org.smallpearl.compiler.*;
import org.smallpearl.compiler.SymbolTable.ModuleEntry;
import org.smallpearl.compiler.SymbolTable.SymbolTable;
import org.smallpearl.compiler.SymbolTable.SymbolTableEntry;
import org.smallpearl.compiler.SymbolTable.VariableEntry;
import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;

public class CheckAssignment extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;
    private ExpressionTypeVisitor m_expressionTypeVisitor;
    private SymbolTableVisitor m_symbolTableVisitor;
    private SymbolTable m_symboltable;
    private SymbolTable m_currentSymbolTable;
    private ModuleEntry m_module;


    public CheckAssignment(String sourceFileName,
                           int verbose,
                           boolean debug,
                           SymbolTableVisitor symbolTableVisitor,
                           ExpressionTypeVisitor expressionTypeVisitor) {

        m_debug = debug;
        m_verbose = verbose;
        m_sourceFileName = sourceFileName;
        m_symbolTableVisitor = symbolTableVisitor;
        m_expressionTypeVisitor = expressionTypeVisitor;
        m_symboltable = symbolTableVisitor.symbolTable;
        m_currentSymbolTable = m_symboltable;

        if (m_verbose > 0) {
            System.out.println( "    Check Assignments");
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The type of the variable given to the left of the assignment sign has to match the type of the  value of the
    // expression, with the following exceptions:
    //  (1) The value of a FIXED variable or an integer, resp., may be assigned to a FLOAT variable.
    //  (2) The precision of a numeric variable to the left of an assignment sign may be greater than the precision of
    //      the value of the expression.
    //  (3) A bit or character string, resp., to the left may have a greater length than the value to be assigned; if
    //      needed, the latter is extended by zeros or spaces, resp., on the right.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    public Void visitAssignment_statement(SmallPearlParser.Assignment_statementContext ctx) {
        if (m_debug) {
            System.out.println("Semantic: visitAssignment_statement");
        }

        String id = null;

        if ( ctx.stringSelection() != null ) {
            if ( ctx.stringSelection().charSelection() != null ) {
                id = ctx.stringSelection().charSelection().ID().getText();
            }
            else  if (ctx.stringSelection().bitSelection() != null) {
                id = ctx.stringSelection().bitSelection().ID().getText();
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
        else {
            id = ctx.ID().getText();
        }

        SymbolTableEntry lhs = m_currentSymbolTable.lookup(id);

        TypeDefinition rhs = m_expressionTypeVisitor.lookupType(ctx.expression());
        ExpressionResult rhs1 = m_expressionTypeVisitor.lookup(ctx.expression());

        if ( lhs instanceof VariableEntry) {
            VariableEntry variable = (VariableEntry) lhs;

            if ( lhs == null ) {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            if ( variable.getLoopControlVariable()) {
                throw new SemanticError(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine(), "Loop control variable cannot be changed");
            }

            if ( rhs == null ) {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            if ( variable.getType() instanceof TypeFloat ) {
                TypeFloat lhs_type = (TypeFloat) variable.getType();

                if ( !(rhs instanceof  TypeFloat || rhs instanceof TypeFixed) ) {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                if ( rhs instanceof TypeFloat ) {
                    TypeFloat rhs_type = (TypeFloat)rhs;

                    if ( rhs_type.getPrecision() >  lhs_type.getPrecision() ) {
                        throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                    }
                }
                else if ( rhs instanceof TypeFixed ) {
                    TypeFixed rhs_type = (TypeFixed) rhs;
                }
            }
            else if ( variable.getType() instanceof TypeFixed ) {
                TypeFixed lhs_type = (TypeFixed) variable.getType();

                if ( rhs instanceof TypeReference) {
                    TypeReference rhs_type = (TypeReference)rhs;
                    if (! (rhs_type.getBaseType() instanceof TypeFixed)) {
                        throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                    }
                }
                else if ( !(rhs instanceof TypeFixed) ) {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }


                int p1 = ((TypeFixed) variable.getType()).getPrecision();
                int p2 = ((TypeFixed)rhs).getPrecision();

                if ( ((TypeFixed) variable.getType()).getPrecision() < ((TypeFixed)rhs).getPrecision() ) {
                  throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
            else if ( variable.getType() instanceof TypeClock ) {
                if ( !(rhs instanceof TypeClock) ) {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
            else if ( variable.getType() instanceof TypeDuration ) {
                if ( !(rhs instanceof TypeDuration) ) {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
            else if ( variable.getType() instanceof TypeBit ) {
                TypeBit lhs_type = (TypeBit) variable.getType();

                if (!(rhs instanceof TypeBit)) {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                TypeBit rhs_type = (TypeBit) rhs;

                if (rhs_type.getPrecision() > lhs_type.getPrecision()) {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
            else if ( variable.getType() instanceof TypeReference ) {
                TypeReference lhs_type = (TypeReference) variable.getType();
                TypeDefinition rhs_type;

                if ( ctx.dereference() == null ) {
                    if ( (rhs1.getVariable() == null) && ( !(rhs1.getType() instanceof TypeTask))) {
                        throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                    }

                    TypeDefinition lt = lhs_type.getBaseType();

                    if ( rhs instanceof TypeReference) {
                        rhs_type = ((TypeReference) rhs).getBaseType();
                    }
                    else {
                        rhs_type = rhs;
                    }

                    if ( !(lt.equals(rhs_type))) {
                        throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                    }
                }
                else {
                    TypeDefinition lt = lhs_type.getBaseType();
                    if ( !(lt.equals(rhs))) {
                        throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                    }
                }
            }
            else if ( variable.getType() instanceof TypeTask ) {
                System.out.println("Semantic: visitAssignment_statement: TASK");
            }
        }
        else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        return null;
    }

    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        org.smallpearl.compiler.SymbolTable.SymbolTableEntry symbolTableEntry = m_currentSymbolTable.lookupLocal(ctx.ID().getText());
        m_currentSymbolTable = ((org.smallpearl.compiler.SymbolTable.ModuleEntry)symbolTableEntry).scope;
        visitChildren(ctx);
        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }
}