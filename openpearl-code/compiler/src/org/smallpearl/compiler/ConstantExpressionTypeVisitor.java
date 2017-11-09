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

public  class ConstantExpressionTypeVisitor extends SmallPearlBaseVisitor<FixedValue> implements SmallPearlVisitor<FixedValue> {

    private int m_verbose;
    private boolean m_debug;
    private SymbolTableVisitor m_symbolTableVisitor;
    private SymbolTable m_symboltable;
    private SymbolTable m_currentSymbolTable;
    private ModuleEntry m_module;
    private ParseTreeProperty<ExpressionResult> m_properties = null;
    private int m_fixed_result = 0;

    public ConstantExpressionTypeVisitor(int verbose, boolean debug, SymbolTableVisitor symbolTableVisitor) {

        m_verbose = verbose;
        m_debug = debug;

        m_debug = true;
        m_verbose = 1;
        
        m_symbolTableVisitor = symbolTableVisitor;
        m_symboltable = symbolTableVisitor.symbolTable;

        if (m_verbose > 0) {
            System.out.println("Semantic Check: Attributing parse tree with constant expression information");
        }

        if (m_debug) {
            System.out.println("Semantic Check: Enabled debugging information");
        }

        LinkedList<ModuleEntry> listOfModules = this.m_symboltable.getModules();

        if (listOfModules.size() > 1) {
            throw new NotYetImplementedException("Multiple modules", 0, 0);
        }

        m_module = listOfModules.get(0);
        m_currentSymbolTable = m_module.scope;
        m_properties = new ParseTreeProperty<ExpressionResult>();
    }

    public TypeDefinition lookupType(ParserRuleContext ctx) {
        return m_properties.get(ctx).getType();
    }

    public ExpressionResult lookup(ParserRuleContext ctx) {
        return m_properties.get(ctx);
    }

    @Override
    public FixedValue visitConstantExpression(SmallPearlParser.ConstantExpressionContext ctx) {
        if (m_debug) {
            System.out.println("ConstantExpressionTypeVisitor: visitConstantExpression");
        }

        m_fixed_result = 0;
        visitChildren(ctx);
        if (m_debug) {
            System.out.println("ConstantExpressionTypeVisitor: visitConstantExpression res="+m_fixed_result);
        }

        return null;
    }

    @Override
    public FixedValue visitConstantFixedExpression(SmallPearlParser.ConstantFixedExpressionContext ctx) {
        if (m_debug) {
            System.out.println("ConstantExpressionTypeVisitor: visitConstantFixedExpression");
        }
        visitChildren(ctx);
        return null;
    }

    @Override
    public FixedValue visitConstantFixedExpressionTerm(SmallPearlParser.ConstantFixedExpressionTermContext ctx) {
        if (m_debug) {
            System.out.println("ConstantExpressionTypeVisitor: visitConstantFixedExpressionTerm");
        }

        visitChildren(ctx);
        return null;
    }

    @Override
    public FixedValue visitConstantFixedExpressionFactor(SmallPearlParser.ConstantFixedExpressionFactorContext ctx) {
        if (m_debug) {
            System.out.println("ConstantExpressionTypeVisitor: visitConstantFixedExpressionFactor");
        }
        visitChildren(ctx);
        return null;
    }

}

/*
constantFixedExpressionFactor
        : ( '+' | '-' )? ( IntegerConstant
        | '(' constantFixedExpression ')'
        | ID )
        ( 'FIT' constantFixedExpression ) ?
        ;
*/