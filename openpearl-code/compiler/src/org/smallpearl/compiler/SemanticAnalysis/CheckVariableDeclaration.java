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

import org.antlr.v4.runtime.tree.ParseTree;
import org.smallpearl.compiler.*;
import org.smallpearl.compiler.SymbolTable.ModuleEntry;
import org.smallpearl.compiler.SymbolTable.SymbolTable;
import org.smallpearl.compiler.SymbolTable.SymbolTableEntry;
import org.smallpearl.compiler.SymbolTable.VariableEntry;
import org.stringtemplate.v4.ST;

import java.util.ArrayList;

public class CheckVariableDeclaration extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;
    private ExpressionTypeVisitor m_expressionTypeVisitor;
    private SymbolTableVisitor m_symbolTableVisitor;
    private SymbolTable m_symboltable;
    private SymbolTable m_currentSymbolTable;
    private ModuleEntry m_module;

    public CheckVariableDeclaration(String sourceFileName,
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
            System.out.println( "    Check Variable Declaration");
        }
    }

    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        if (m_debug) {
            System.out.println( "Semantic: Check Variable Declaration: visitModule");
        }

        org.smallpearl.compiler.SymbolTable.SymbolTableEntry symbolTableEntry = m_currentSymbolTable.lookupLocal(ctx.ID().getText());
        m_currentSymbolTable = ((ModuleEntry)symbolTableEntry).scope;
        visitChildren(ctx);
        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        if (m_debug) {
            System.out.println( "Semantic: Check Variable Declaration: visitProcedureDeclaration");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitProcedureBody(SmallPearlParser.ProcedureBodyContext ctx) {
        if(ctx != null && ctx.children != null ) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ScalarVariableDeclarationContext) {
                    visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext) c);
                } else if (c instanceof SmallPearlParser.StatementContext) {
                    visitStatement((SmallPearlParser.StatementContext) c);
                } else if (c instanceof SmallPearlParser.DationDeclarationContext) {
                    SmallPearlParser.DationDeclarationContext declctx =  (SmallPearlParser.DationDeclarationContext) c;
                    throw new DationDeclarationNotAllowedHereException(declctx.getText(), declctx.start.getLine(), declctx.start.getCharPositionInLine());
                }
            }
        }

        return null;
    }

    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        if (m_debug) {
            System.out.println( "Semantic: Check Variable Declaration: visitTaskDeclaration");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        if (m_debug) {
            System.out.println( "Semantic: Check Variable Declaration: visitBlock_statement");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        if (m_debug) {
            System.out.println( "Semantic: Check Variable Declaration: visitLoopStatement");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitScalarVariableDeclaration(SmallPearlParser.ScalarVariableDeclarationContext ctx) {
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
        ArrayList<ConstantValue> initElementList = null;

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
                    getInitialisationAttribute((SmallPearlParser.InitialisationAttributeContext) c);
                }
            }

            if (initElementList != null && identifierDenotationList.size() != initElementList.size()) {
                throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            // TODO: Check Type compability!
            for (int i = 0; i < identifierDenotationList.size(); i++) {
                if (initElementList != null) {
                    ConstantValue value = initElementList.get(i);
                }
            }
        }

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

    private ArrayList<ConstantValue> getInitialisationAttribute(SmallPearlParser.InitialisationAttributeContext ctx) {
        ArrayList<ConstantValue> initList = new ArrayList<ConstantValue>();
        if (ctx != null) {
            for (int i = 0; i < ctx.initElement().size(); i++) {
                // TODO: expression
//                initList.add(getInitElement(ctx.initElement(i).constant()));
            }
        }

        return null;
    }

    private ConstantValue getInitElement(SmallPearlParser.ConstantContext ctx) {
        if (ctx != null)
        {
            if (ctx.fixedConstant() != null)
            {
                Integer value;
                Integer sign = 1;

                value = Integer.parseInt(ctx.fixedConstant().IntegerConstant().getText());

                if (ctx.getChildCount() > 1)
                {
                    if (ctx.getChild(0).getText().equals("-"))
                    {
                        value = -value;
                    }
                }

                if (Integer.toBinaryString(Math.abs(value)).length() < 31)
                {
                    value.toString();
                }
                else
                {
                    // value.toString() + "LL";
                }
            }
            else if (ctx.durationConstant() != null)
            {
                visitDurationConstant(ctx.durationConstant());
            }
            else if (ctx.timeConstant() != null)
            {
                visitTimeConstant(ctx.timeConstant());
            }
            else if (ctx.floatingPointConstant() != null) {
                Double value;
                Integer sign = 1;

                value = Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().getText());

                if (ctx.getChildCount() > 1)
                {
                    if (ctx.getChild(0).getText().equals("-"))
                    {
                        value = -value;
                    }
                }

                Float f11;
//                new org.smallpearl.compiler.ConstantFloatValue(f11);
                return null;
            }
            else if (ctx.StringLiteral() != null)
            {
                String s = ctx.StringLiteral().toString();

                if (s.startsWith("'"))
                {
                    s = s.substring(1, s.length());
                }

                if (s.endsWith("'"))
                {
                    s = s.substring(0, s.length() - 1);
                }
            }
            else if (ctx.bitStringConstant() != null)
            {
                // constant.add("BitStringConstant", getBitStringConstant(ctx));
            }
        }

        return null;
    }
    @Override
    public Void visitArrayVariableDeclaration(SmallPearlParser.ArrayVariableDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println( "Semantic: Check Variable Declaration: visitArrayVariableDeclaration");
        }

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ArrayDenotationContext) {
                    visitArrayDenotation((SmallPearlParser.ArrayDenotationContext) c);
                }
            }
        }

        return null;
    }

    @Override
    public Void visitArrayDenotation(SmallPearlParser.ArrayDenotationContext ctx) {
        for (int i = 0; i < ctx.ID().size(); i++) {
            SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.ID().get(i).toString());

            if ( entry == null || !(entry instanceof VariableEntry)) {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            VariableEntry variableEntry = (VariableEntry)entry;

            if ( variableEntry.getType() instanceof TypeArray) {
                ArrayList<ST> initElementList = null;

                if ( variableEntry.getType() instanceof TypeArray) {
                    TypeArray type = (TypeArray)variableEntry.getType();
                }
                else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                if (ctx.arrayInitialisationAttribute() != null) {
                }

            }
        }

        return null;
    }

}