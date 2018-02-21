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
 *     derived from this software without specific prior written permissision.
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
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.smallpearl.compiler.SymbolTable.*;

import java.util.LinkedList;

public  class FixUpSymbolTableVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;
    private boolean m_debug;
    private SymbolTableVisitor m_symbolTableVisitor;
    private SymbolTable m_symboltable;
    private SymbolTable m_currentSymbolTable;
    private ExpressionTypeVisitor m_expressionTypeVisitor;
    private ConstantPoolVisitor m_constantPoolVisitor;
    private ModuleEntry m_module;
    private ParseTreeProperty<ExpressionResult> m_properties = null;

    public FixUpSymbolTableVisitor(int verbose,
                                   boolean debug,
                                   SymbolTableVisitor symbolTableVisitor,
                                   ExpressionTypeVisitor expressionTypeVisitor,
                                   ConstantPoolVisitor   constantPoolVisitor) {

        m_verbose = verbose;
        m_debug = debug;

        m_symbolTableVisitor = symbolTableVisitor;
        m_symboltable = symbolTableVisitor.symbolTable;

        m_expressionTypeVisitor = expressionTypeVisitor;
        m_constantPoolVisitor  = constantPoolVisitor;

        LinkedList<ModuleEntry> listOfModules = this.m_symboltable.getModules();

        if (listOfModules.size() > 1) {
            throw new NotYetImplementedException("Multiple modules", 0, 0);
        }

        m_module = listOfModules.get(0);
        m_currentSymbolTable = m_module.scope;
        m_properties = new ParseTreeProperty<ExpressionResult>();
    }


    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        if (m_verbose > 0) {
            System.out.println("FixUpSymbolTableVisitor: visitModule");
        }

//        this.m_currentSymbolTable = this.symbolTable.newLevel(moduleEntry);

        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }


    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println("FixUpSymbolTableVisitor: vistTaskDeclaration");
        }

        String s = ctx.ID().getText();
        SymbolTableEntry entry = this.m_currentSymbolTable.lookupLocal(ctx.ID().getText());



        if (entry != null) {
            if (entry instanceof TaskEntry) {
                m_currentSymbolTable = ((TaskEntry) entry).scope;
                visitChildren(ctx);
                this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else {
            m_symboltable.dump();
            m_currentSymbolTable.dump();

            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println("FixUpSymbolTableVisitor: visitProcedureDeclaration");
        }

        SymbolTableEntry entry = this.m_currentSymbolTable.lookupLocal(ctx.ID().getText());

        if (entry != null) {
            if (entry instanceof ProcedureEntry) {
                m_currentSymbolTable = ((ProcedureEntry) entry).scope;

                visitChildren(ctx);
                this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        if (m_verbose > 0) {
            System.out.println("FixUpSymbolTableVisitor: visitBlock_statement");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        int precision;
        VariableEntry var = null;

        if (m_verbose > 0) {
            System.out.println("FixUpSymbolTableVisitor: visitLoopStatement");
        }

        m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);

        if (ctx.loopStatement_for() != null) {
            SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.loopStatement_for().ID().getText());

            if (entry != null && entry instanceof VariableEntry) {
                var = (VariableEntry) entry;
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        TypeFixed fromType = null;
        TypeFixed toType = null;
        TypeFixed byType = null;

        ExpressionResult fromRes = null;
        ExpressionResult toRes = null;
        ExpressionResult byRes = null;

        int fromPrecision = 1;
        int toPrecision   = Defaults.FIXED_LENGTH;
        int byPrecision   = 1;

        if (ctx.loopStatement_from() != null) {
            fromRes = m_expressionTypeVisitor.lookup(ctx.loopStatement_from().expression());
            fromPrecision = ((TypeFixed)fromRes.getType()).getPrecision();
            fromType = new TypeFixed(fromPrecision);
        }

        if (ctx.loopStatement_to() != null) {
            toRes = m_expressionTypeVisitor.lookup(ctx.loopStatement_to().expression());
            toPrecision = ((TypeFixed)toRes.getType()).getPrecision();
            toType = new TypeFixed(toPrecision);
        }

        if (ctx.loopStatement_by() != null) {
            byRes = m_expressionTypeVisitor.lookup(ctx.loopStatement_by().expression());
            byPrecision = ((TypeFixed)byRes.getType()).getPrecision();
            byType = new TypeFixed(byPrecision);
        }

        precision = m_currentSymbolTable.lookupDefaultFixedLength();

        if ( byPrecision > fromPrecision || byPrecision > toPrecision) {
            throw new LoopByOutOfRangeException(null, ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( fromType != null) {
            if ( toType != null ) {
                precision = Math.max(((TypeFixed)fromRes.getType()).getPrecision(),((TypeFixed)toRes.getType()).getPrecision());
            }
            else {
                precision = ((TypeFixed)fromRes.getType()).getPrecision();
            }
        }
        else if ( toType != null ) {
            precision = ((TypeFixed)toRes.getType()).getPrecision();
        }

        if ( var != null) {
            TypeDefinition varType = var.getType();
            if (varType != null && varType instanceof TypeFixed) {
                TypeFixed typ = (TypeFixed) varType;
                typ.setPrecision(precision);
            }
        }

        m_constantPoolVisitor.add(new ConstantFixedValue(0, precision));
        m_constantPoolVisitor.add(new ConstantFixedValue(1, precision));

        if ( fromType == null ) {
            m_constantPoolVisitor.add(new ConstantFixedValue(1,precision));
        }

        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }
}