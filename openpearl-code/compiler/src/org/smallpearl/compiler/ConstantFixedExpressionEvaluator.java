/*
 * [The "BSD license"]
 *  Copyright (c) 2017 Marcel Schaible
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

import org.antlr.v4.runtime.tree.ParseTree;
import org.smallpearl.compiler.SymbolTable.ModuleEntry;
import org.smallpearl.compiler.SymbolTable.SymbolTable;
import org.smallpearl.compiler.SymbolTable.SymbolTableEntry;
import org.smallpearl.compiler.SymbolTable.VariableEntry;

public  class ConstantFixedExpressionEvaluator extends SmallPearlBaseVisitor<ConstantFixedValue> implements SmallPearlVisitor<ConstantFixedValue> {

    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;
    private SymbolTable m_currentSymbolTable;
    private ConstantExpressionEvaluatorVisitor  m_constantExpressionEvaluatorVisitor;

    public ConstantFixedExpressionEvaluator(int verbose,
                                            boolean debug,
                                            SymbolTable symbolTable,
                                            ConstantExpressionEvaluatorVisitor constantExpressionEvaluatorVisitor) {

        m_debug = debug;
        m_verbose = verbose;
        m_currentSymbolTable = symbolTable;
        m_constantExpressionEvaluatorVisitor = constantExpressionEvaluatorVisitor;

        if (m_verbose > 0) {
            System.out.println( "    ConstantFixedExpressionEvaluator");
        }
    }

    @Override
    public ConstantFixedValue visitConstantExpression(SmallPearlParser.ConstantExpressionContext ctx) {
        ConstantFixedValue value;

        if (m_debug) {
            System.out.println("ConstantFixedExpressionEvaluator: visitConstantExpression");
        }

        value = visitChildren(ctx);

        System.out.println("ConstantFixedExpressionEvaluator: value="+value);

        return value;
    }

    @Override
    public ConstantFixedValue visitConstantFixedExpression(SmallPearlParser.ConstantFixedExpressionContext ctx) {
        ConstantFixedValue value = null;

        if (m_debug) {
            System.out.println("ConstantFixedExpressionEvaluator: visitConstantFixedExpression");
        }

        value = visitConstantFixedExpressionTerm(ctx.constantFixedExpressionTerm());

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.AdditiveConstantFixedExpressionTermContext) {
                ConstantFixedValue rhs =  visit((SmallPearlParser.AdditiveConstantFixedExpressionTermContext)c);
                long res = value.getValue() + rhs.getValue();
                ConstantFixedValue v = new ConstantFixedValue(res);
                value = v;
            }
            else if (c instanceof SmallPearlParser.SubtractiveConstantFixedExpressionTermContext) {
                ConstantFixedValue rhs =  visit((SmallPearlParser.SubtractiveConstantFixedExpressionTermContext)c);
                long res = value.getValue() - rhs.getValue();
                ConstantFixedValue v = new ConstantFixedValue(res);
                value = v;
            }
        }

        return value;
    }

    @Override public ConstantFixedValue visitAdditiveConstantFixedExpressionTerm(SmallPearlParser.AdditiveConstantFixedExpressionTermContext ctx)
    {
        return visitChildren(ctx);
    }

    @Override
    public ConstantFixedValue visitConstantFixedExpressionTerm(SmallPearlParser.ConstantFixedExpressionTermContext ctx) {
        ConstantFixedValue value = null;

        if (m_debug) {
            System.out.println("ConstantFixedExpressionEvaluator: visitConstantFixedExpressionTerm");
        }

        value = visitConstantFixedExpressionFactor(ctx.constantFixedExpressionFactor());

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.MultiplicationConstantFixedExpressionTermContext) {
                ConstantFixedValue rhs =  visit((SmallPearlParser.MultiplicationConstantFixedExpressionTermContext)c);
                long res = value.getValue() * rhs.getValue();
                ConstantFixedValue v = new ConstantFixedValue(res);
                value = v;
            }
            else if (c instanceof SmallPearlParser.DivisionConstantFixedExpressionTermContext) {
                ConstantFixedValue rhs =  visit((SmallPearlParser.DivisionConstantFixedExpressionTermContext)c);

                if ( rhs.getValue() != 0 ) {
                    long res = value.getValue() / rhs.getValue();
                    ConstantFixedValue v = new ConstantFixedValue(res);
                    value = v;
                } else {
                    throw new DivisionByZeroException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
            else if (c instanceof SmallPearlParser.RemainderConstantFixedExpressionTermContext) {
                ConstantFixedValue rhs =  visit((SmallPearlParser.RemainderConstantFixedExpressionTermContext)c);

                if ( rhs.getValue() != 0 ) {
                    long res = value.getValue() % rhs.getValue();
                    ConstantFixedValue v = new ConstantFixedValue(res);
                    value = v;
                } else {
                    throw new DivisionByZeroException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }

        }

        return value;
    }

/*
////////////////////////////////////////////////////////////////////////////////
// Factor ::=
//   [+ | -] {  Integer
//             | ( ConstantFIXEDExpression )
//             | TOFIXED { CharacterStringConstant§OfLength1 | BitStringConstant }
//             | Identifier§NamedFIXEDConstant
//           }
// [ FIT ConstantFIXEDExpression ]
////////////////////////////////////////////////////////////////////////////////

    constantFixedExpressionFactor
        : Sign? (   IntegerConstant
        | '(' constantFixedExpression ')'
            | ID )
    constantFixedExpressionFit?
    ;

    constantFixedExpressionFit
        :
                'FIT' constantFixedExpression
    ;
*/

    @Override
    public ConstantFixedValue visitConstantFixedExpressionFactor(SmallPearlParser.ConstantFixedExpressionFactorContext ctx) {
        ConstantFixedValue value = null;
        int sign = 1;

        if (m_debug) {
            System.out.println("ConstantFixedExpressionEvaluator: visitConstantFixedExpressionFactor");
        }

        if ( ctx.Sign() != null) {
            if ( ctx.Sign().getText().equals("-")) {
                sign = -1;
            }
        }

        if ( ctx.IntegerConstant() != null) {
            int r = sign * Integer.parseInt(ctx.IntegerConstant().toString());
            value = new ConstantFixedValue(r);
        }
        else if ( ctx.ID() != null ) {
            SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.ID().toString());

            if ( entry == null ) {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            if ( entry instanceof VariableEntry ) {
                VariableEntry variableEntry = (VariableEntry)entry;
                if ( !variableEntry.getAssigmentProtection() ) {
                    throw new ConstantExpectedException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                if ( !(variableEntry.getType() instanceof TypeFixed )) {
                    throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                System.out.println("****:"+variableEntry.getConstantCtx());

                if ( variableEntry.getConstantCtx() instanceof SmallPearlParser.ConstantExpressionContext) {
                    SmallPearlParser.ConstantExpressionContext c = (SmallPearlParser.ConstantExpressionContext)variableEntry.getConstantCtx();

                    ConstantValue v = m_constantExpressionEvaluatorVisitor.lookup(c);

                    if ( v instanceof ConstantFixedValue) {
                        value = (ConstantFixedValue)v;
                    }
                }
                else if ( variableEntry.getConstantCtx() instanceof SmallPearlParser.ConstantContext) {
                    SmallPearlParser.ConstantContext c = (SmallPearlParser.ConstantContext)variableEntry.getConstantCtx();

                    if ( c.StringLiteral() != null) {
                        System.out.println("****StringLiteral***");
                    }

                    else if ( c.IntegerConstant() != null ) {
                        int v = Integer.parseInt(c.IntegerConstant().getText());
                        value = new ConstantFixedValue(v);

                    }
                    else if ( c.floatingPointConstant() != null ) {
                        System.out.println("****FloatingPointConstant***");
                    }
                    else if ( c.durationConstant() != null ) {
                        System.out.println("****durationConstant***");
                    }
                    else if ( c.bitStringConstant() != null ) {
                        System.out.println("****bitStringConstant***");
                    }
                    else if ( c.timeConstant() != null ) {
                        System.out.println("****timeConstant***");
                    }
                }
            }
        }
        else if ( ctx.constantFixedExpression() != null ) {
            value = visitConstantFixedExpression(ctx.constantFixedExpression());
        }

        if ( ctx.constantFixedExpressionFit() != null ) {
            throw new NotYetImplementedException("Constant Expression Operator 'FIT'", 0, 0);
        }

        System.out.println("ConstantExpressionEvaluatorVisitor: value="+value);

        return value;
    }

/**
 constantFixedExpressionFit
 :
         'FIT' constantFixedExpression
    ;
**/

/**
////////////////////////////////////////////////////////////////////////////////
// ConstantFIXEDExpression ::=
//   Term [ { + | - } Term ] ...
////////////////////////////////////////////////////////////////////////////////
    constantFixedExpression
        : constantFixedExpressionTerm (op='+' constantFixedExpressionTerm )*
            | constantFixedExpressionTerm (op='-' constantFixedExpressionTerm )*
    ;
**/
/**
////////////////////////////////////////////////////////////////////////////////
// Term ::=
//   Factor [ {∗ | // | REM } Factor ] ...
////////////////////////////////////////////////////////////////////////////////
    constantFixedExpressionTerm
        : constantFixedExpressionFactor ( op='*' constantFixedExpressionFactor )*
            | constantFixedExpressionFactor ( op='//' constantFixedExpressionFactor )*
            | constantFixedExpressionFactor ( op='REM' constantFixedExpressionFactor )*
    ;
**/
}
