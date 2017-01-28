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
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.stringtemplate.v4.ST;

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
        m_debug = true;

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
        int constantBitNo = 0;

        boolean found = false;

        for (i = 0; i < constantPool.size(); i++) {
            if ( value instanceof ConstantFixedValue) {
                if (constantPool.get(i) instanceof ConstantFixedValue) {
                    Long a = ((ConstantFixedValue)(value)).getValue();
                    Long b = ((ConstantFixedValue)(constantPool.get(i))).getValue();

                    if ( a.equals(b) ) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantFloatValue) {
                if (constantPool.get(i) instanceof ConstantFloatValue) {
                    if ( Double.compare( ((ConstantFloatValue)(value)).getValue(), ((ConstantFloatValue)(constantPool.get(i))).getValue()) == 0) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantCharacterValue) {
                if (constantPool.get(i) instanceof ConstantCharacterValue) {
                    String s1 = ((ConstantCharacterValue)(value)).getValue();
                    String s2 = ((ConstantCharacterValue)(constantPool.get(i))).getValue();

                    if ( s1.equals(s2)) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantBitValue) {
                if (constantPool.get(i) instanceof ConstantBitValue) {
                    constantBitNo = constantBitNo + 1;
                    String s1 = ((ConstantBitValue)(value)).getValue();
                    String s2 = ((ConstantBitValue)(constantPool.get(i))).getValue();

                    if ( s1.equals(s2)) {
                        found = true;
                        break;
                    }
                }
            }
        }

        if (!found) {
            if ( value instanceof ConstantBitValue) {
                constantBitNo = constantBitNo + 1;
                ((ConstantBitValue) (value)).setNo(constantBitNo);
            }

            constantPool.add(value);
        }

        return null;
    }

//    public ConstantValue lookup(String value) {
//        int i;
//        boolean found = false;
//
//        for (i = 0; i < constantPool.size(); i++) {
//            if ( value instanceof ConstantFixedValue) {
//                if (constantPool.get(i) instanceof ConstantFixedValue) {
//                    Long a = ((ConstantFixedValue)(value)).getValue();
//                    Long b = ((ConstantFixedValue)(constantPool.get(i))).getValue();
//
//                    if ( a.equals(b) ) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//            else if ( value instanceof ConstantFloatValue) {
//                if (constantPool.get(i) instanceof ConstantFloatValue) {
//                    if ( Double.compare( ((ConstantFloatValue)(value)).getValue(), ((ConstantFloatValue)(constantPool.get(i))).getValue()) == 0) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//            else if ( value instanceof ConstantCharacterValue) {
//                if (constantPool.get(i) instanceof ConstantCharacterValue) {
//                    String s1 = ((ConstantCharacterValue)(value)).getValue();
//                    String s2 = ((ConstantCharacterValue)(constantPool.get(i))).getValue();
//
//                    if ( s1.equals(s2)) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//            else if ( value instanceof ConstantBitValue) {
//                if (constantPool.get(i) instanceof ConstantBitValue) {
//                    constantBitNo = constantBitNo + 1;
//                    String s1 = ((ConstantBitValue)(value)).getValue();
//                    String s2 = ((ConstantBitValue)(constantPool.get(i))).getValue();
//
//                    if ( s1.equals(s2)) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//
//        }
//
//        if (!found) {
//            if ( value instanceof ConstantBitValue) {
//                constantBitNo = constantBitNo + 1;
//                ((ConstantBitValue) (value)).setNo(constantBitNo);
//            }
//
//            constantPool.add(value);
//        }
//
//        return null;
//    }

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
            add(new ConstantCharacterValue(ctx.StringLiteral().toString()));
        } else if (ctx.FloatingPointConstant() != null) {
            try {
                Double value = Double.parseDouble(ctx.FloatingPointConstant().toString());
                String strval = ctx.FloatingPointConstant().toString();
                Integer precision = 24;
                add(new ConstantFloatValue(value,precision));
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
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
                            System.out.println("fixed:(" + value + "," + precision + ")");
                        } catch (NumberFormatException ex) {
                            throw new NumberOutOfRangeException(ctx.getText(), literal_ctx.start.getLine(), literal_ctx.start.getCharPositionInLine());
                        }
                    } else if (literal_ctx.StringLiteral() != null) {
                        System.out.println("string:(" + literal_ctx.StringLiteral().toString() + ")");
                    } else if (literal_ctx.FloatingPointConstant() != null) {
                        try {
                            Float value = -1 * Float.parseFloat(literal_ctx.FloatingPointConstant().toString());
                            Integer precision = 24;
                            add(new ConstantFloatValue(value));
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

    @Override
    public Void visitVariableDenotation(SmallPearlParser.VariableDenotationContext ctx) {
        ArrayList<ST> initElementList = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.InitialisationAttributeContext) {
                    initElementList = getInitialisationAttribute((SmallPearlParser.InitialisationAttributeContext) c);
                }
            }

//            for (int i = 0; i < identifierDenotationList.size(); i++) {
//
//                if (initElementList != null)
//                    initElementList.get(i);
//            }
        }

        return null;
    }
    private ArrayList<ST> getInitialisationAttribute(SmallPearlParser.InitialisationAttributeContext ctx) {
        ArrayList<ST> initElementList = new ArrayList<ST>();

        if (ctx != null) {
            for (int i = 0; i < ctx.initElement().size(); i++) {
                // TODO: expression
                initElementList.add(getInitElement(ctx.initElement(i).constant()));
            }
        }

        return initElementList;
    }

    private ST getInitElement(SmallPearlParser.ConstantContext ctx) {
        if (ctx != null) {
            if (ctx.IntegerConstant() != null) {
                Integer value;
                Integer sign = 1;

                value = Integer.parseInt(ctx.IntegerConstant().getText());

                if (ctx.getChildCount() > 1) {
                    if (ctx.getChild(0).getText().equals("-")) {
                        value = -value;
                    }
                }

                if (Integer.toBinaryString(Math.abs(value)).length() < 31) {
//                                        integerConstant.add("value", value.toString());
                } else {
//                    integerConstant.add("value", value.toString() + "LL");
                }

//                constant.add("IntegerConstant", integerConstant);
            } else if (ctx.durationConstant() != null) {
//                durationConstant.add("value", visitDurationConstant(ctx.durationConstant()));
//                constant.add("DurationConstant", durationConstant);
            } else if (ctx.timeConstant() != null) {
//                timeConstant.add("value", visitTimeConstant(ctx.timeConstant()));
//                constant.add("TimeConstant", timeConstant);
            } else if (ctx.FloatingPointConstant() != null) {
                Double value;
                Integer sign = 1;

                value = Double.parseDouble(ctx.FloatingPointConstant().getText());

                if (ctx.getChildCount() > 1) {
                    if (ctx.getChild(0).getText().equals("-")) {
                        value = -value;
                    }
                }

//                constant.add("FloatingPointConstant", value);
            } else if (ctx.StringLiteral() != null) {
                String s = ctx.StringLiteral().toString();

                if (s.startsWith("'")) {
                    s = s.substring(1, s.length());
                }

                if (s.endsWith("'")) {
                    s = s.substring(0, s.length() - 1);
                }

//                constant.add("StringConstant", stringConstant);
            } else if (ctx.bitStringConstant() != null) {
//                constant.add("BitStringConstant", getBitStringConstant(ctx));
            }
        }

        return null;
    }

    private ST getBitStringConstant(SmallPearlParser.ConstantContext ctx) {
//        int nb = 1;
//        Long l = convertBitStringToLong(ctx.bitStringConstant().BitStringLiteral().toString());
//
//        // walk up the AST and get VariableDenotationContext:
//        ParserRuleContext sctx = ctx.getParent();
//        while (sctx != null && !(sctx instanceof SmallPearlParser.VariableDenotationContext)) {
//            sctx = sctx.getParent();
//        }
//
//        if (sctx != null) {
//            SmallPearlParser.TypeAttributeContext typeAttributeContext = ((SmallPearlParser.VariableDenotationContext) sctx).typeAttribute();
//            if (typeAttributeContext.simpleType() != null) {
//                SmallPearlParser.SimpleTypeContext simpleTypeContext = typeAttributeContext.simpleType();
//
//                if (simpleTypeContext.typeBitString() != null) {
//                    SmallPearlParser.TypeBitStringContext typeBitStringContext = simpleTypeContext.typeBitString();
//
//                    if (typeBitStringContext.IntegerConstant() != null) {
//                        nb = Integer.valueOf(typeBitStringContext.IntegerConstant().toString());
//                    }
//                }
//            }
//
//        } else {
//            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
//        }
//
//        return formatBitStringConstant(l,nb);
        return null;
    }


}
