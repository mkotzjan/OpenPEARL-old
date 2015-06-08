 /*
 * [The "BSD license"]
 *  Copyright (c) 2012-2013 Marcel Schaible
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
import org.antlr.v4.runtime.tree.ParseTreeWalker;
import org.antlr.v4.runtime.ANTLRFileStream;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.tree.TerminalNode;
import org.antlr.v4.runtime.TokenStream;
import org.stringtemplate.v4.*;

import static java.lang.System.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.List;

public class BuildSymbolTableVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;

    public BuildSymbolTableVisitor(int verbose) {

        m_verbose = verbose;

        if (m_verbose > 0) {
            System.out.println( "Building symbol table");
        }
    }

    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        SymbolTable symtab = SymbolTable.getSymbolTable();
        symtab.defineModule(new Symbol(ctx.ID().getSymbol()));
        return visitChildren(ctx);
    }

    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        int priority = 255;

        if ( ctx.priority() != null ) {
            priority = Integer.parseInt(ctx.priority().IntegerConstant().getText());
        }

        SymbolTable symtab = SymbolTable.getSymbolTable();
        symtab.defineTask(new Symbol(ctx.ID().getSymbol()), priority);
        visitChildren(ctx);
        symtab.popScope();
        return null;
    }


    @Override
    public Void visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        SymbolTable symtab = SymbolTable.getSymbolTable();
        symtab.defineProcedure(new Symbol(ctx.ID().getSymbol()));
        visitChildren(ctx);
        symtab.popScope();
        return null;
    }

    @Override
    public Void visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        SymbolTable symtab = SymbolTable.getSymbolTable();
        symtab.defineBlock(new Symbol(ctx.getStart()));
        visitChildren(ctx);
        symtab.popScope();
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
        ArrayList<ST> initElementList = null;

        SymbolTable symtab = SymbolTable.getSymbolTable();

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

            for (int i = 0; i < identifierDenotationList.size(); i++) {
/*
                ST v = group.getInstanceOf("VariableDeclaration");
                v.add("name", identifierDenotationList.get(i));
                v.add("TypeAttribute", typeAttribute);
                v.add("global", hasGlobalAttribute);
                v.add("inv", hasAllocationProtection);

                if (initElementList != null)
                    v.add("InitElement", initElementList.get(i));

                variableDenotation.add("decl", v);


*/
                symtab.defineVar(new Symbol(identifierDenotationList.get(i),0,0,0));



            }
        }

        return null;
    }

    /**
     * Returns a list of identifier denotations.
     * <p>
     * @see
     * @param  ctx  an IdentifierDenotationContext
     * @return      list of identifier names
     */
    private ArrayList<String> getIdentifierDenotation(SmallPearlParser.IdentifierDenotationContext ctx) {
        ArrayList<String> identifierDenotationList = new ArrayList();

        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                identifierDenotationList.add(ctx.ID().get(i).toString());
            }
        }

        return identifierDenotationList;
    }

    @Override
    public Void visitTypeAttribute(SmallPearlParser.TypeAttributeContext ctx) {
        if (ctx.simpleType() != null) {
            visitSimpleType(ctx.simpleType());
        }
        return null;
    }

    @Override
    public Void visitSemaTry(SmallPearlParser.SemaTryContext ctx) {
        SymbolTable symtab = SymbolTable.getSymbolTable();
        TemporarySemaphoreArray semas = new TemporarySemaphoreArray();

        for (int i = 0; i < ctx.ID().size(); i++) {
            semas.add(ctx.ID(i).toString());
        }

        symtab.defineTemporarySemaphoreArray(semas);

        return null;
    }

    @Override
    public Void visitSemaRequest(SmallPearlParser.SemaRequestContext ctx) {
        SymbolTable symtab = SymbolTable.getSymbolTable();
        TemporarySemaphoreArray semas = new TemporarySemaphoreArray();

        for (int i = 0; i < ctx.ID().size(); i++) {
            semas.add(ctx.ID(i).toString());
        }

        symtab.defineTemporarySemaphoreArray(semas);

        return null;
    }

    @Override
    public Void visitSemaRelease(SmallPearlParser.SemaReleaseContext ctx) {
        SymbolTable symtab = SymbolTable.getSymbolTable();
        TemporarySemaphoreArray semas = new TemporarySemaphoreArray();

        for (int i = 0; i < ctx.ID().size(); i++) {
            semas.add(ctx.ID(i).toString());
        }

        symtab.defineTemporarySemaphoreArray(semas);

        return null;
    }

}