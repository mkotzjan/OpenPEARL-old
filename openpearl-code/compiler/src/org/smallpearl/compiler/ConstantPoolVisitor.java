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

import org.antlr.v4.runtime.ANTLRFileStream;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import java.util.ArrayList;
import java.util.List;


public  class ConstantPoolVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;
    private boolean m_debug;

    private ParseTreeProperty<TypeDefinition> m_properties = null;
    static List<ConstantValue> constantPool = new ArrayList<ConstantValue>();

    public ConstantPoolVisitor(int verbose, boolean debug) {

        m_verbose = verbose;
        m_debug = debug;

        if (m_verbose > 0) {
            System.out.println("Semantic Check: Create ConstantPool");
        }

        if (m_debug) {
            System.out.println("Semantic Check: Enabled debugging information");
        }

        m_properties = new ParseTreeProperty<TypeDefinition>();

        // Add commonly used constants:
        add(new ConstantFixedValue(0));
        add(new ConstantFixedValue(1));
        add(new ConstantFixedValue(-1));
    }

    private Void add(ConstantValue value) {
        int i;
        boolean found = false;

        for (i = 0; i < constantPool.size(); i++) {
            if ( value instanceof ConstantFixedValue) {
                if (constantPool.get(i) instanceof ConstantFixedValue) {
                    if ( ((ConstantFixedValue)(value)).getValue() == ((ConstantFixedValue)(constantPool.get(i))).getValue()) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantFloatValue) {
                if (constantPool.get(i) instanceof ConstantFloatValue) {
                    if ( ((ConstantFloatValue)(value)).getValue() == ((ConstantFloatValue)(constantPool.get(i))).getValue()) {
                        found = true;
                        break;
                    }
                }
            }
        }

        if (!found) {
            constantPool.add(value);
        }

        return null;
    }

    @Override
    public Void visitLiteral(SmallPearlParser.LiteralContext ctx) {
        if (m_debug)
            System.out.println("ConstantPoolVisitor: visitLiteral");

        if (ctx.IntegerConstant() != null) {
            try {
                Integer value = Integer.parseInt(ctx.IntegerConstant().toString());
                Integer precision = Integer.toBinaryString(Math.abs(value)).length();
                add(new ConstantFixedValue(value));
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.StringLiteral() != null) {
            System.out.println("string:(" + ctx.StringLiteral().toString() + ")");
        } else if (ctx.FloatingPointConstant() != null) {
            try {
                Float value = Float.parseFloat(ctx.FloatingPointConstant().toString());
                Integer precision = 24;
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

        } else if (ctx.timeConstant() != null) {
        } else if (ctx.durationConstant() != null) {
        }
        return null;
    }

    @Override
    public Void visitUnarySubtractiveExpression(SmallPearlParser.UnarySubtractiveExpressionContext ctx) {
        if (m_debug)
            System.out.println("ConstantPoolVisitor: visitUnarySubtractiveExpression");

        if (ctx.getChild(1) instanceof SmallPearlParser.BaseExpressionContext) {
            SmallPearlParser.BaseExpressionContext base_ctx = (SmallPearlParser.BaseExpressionContext) (ctx.getChild(1));

            if (base_ctx.primaryExpression() != null) {
                SmallPearlParser.PrimaryExpressionContext primary_ctx = base_ctx.primaryExpression();

                if (primary_ctx.getChild(0) instanceof SmallPearlParser.LiteralContext) {
                    SmallPearlParser.LiteralContext literal_ctx = (SmallPearlParser.LiteralContext) (primary_ctx.getChild(0));

                    if (literal_ctx.IntegerConstant() != null) {
                        try {
                            Integer value = -1 * Integer.parseInt(literal_ctx.IntegerConstant().toString());
                            Integer precision = Integer.toBinaryString(Math.abs(value)).length() + 1;
                            add(new ConstantFixedValue(value));
                        } catch (NumberFormatException ex) {
                            throw new NumberOutOfRangeException(ctx.getText(), literal_ctx.start.getLine(), literal_ctx.start.getCharPositionInLine());
                        }
                    } else if (literal_ctx.StringLiteral() != null) {
                        System.out.println("string:(" + literal_ctx.StringLiteral().toString() + ")");
                    } else if (literal_ctx.FloatingPointConstant() != null) {
                        try {
                            Float value = -1 * Float.parseFloat(literal_ctx.FloatingPointConstant().toString());
                            Integer precision = 24;
                            System.out.println("float:(" + value + "," + precision + ")");
                        } catch (NumberFormatException ex) {
                            throw new NumberOutOfRangeException(ctx.getText(), literal_ctx.start.getLine(), literal_ctx.start.getCharPositionInLine());
                        }
                    } else if (literal_ctx.timeConstant() != null) {
                        System.out.println("time:(" + ")");
                    } else if (literal_ctx.durationConstant() != null) {
                        System.out.println("duration:(" + ")");
                    }
                }
            }
        }
        return null;
    }

    public Void dump(){
        int i;
        System.out.println("");
        System.out.println("ConstantPool:");
        System.out.println("  Entries:" + constantPool.size());

        for ( i = 0; i < constantPool.size(); i++) {
            System.out.println( "    " + constantPool.get(i).toString());
        }

        return null;
    }
}
