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

import org.antlr.runtime.tree.ParseTree;
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.stringtemplate.v4.ST;

public  class ExpressionTypeVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;
    private boolean m_debug;

    private ParseTreeProperty<TypeDefinition> m_properties = null;

    public ExpressionTypeVisitor(int verbose, boolean debug) {

        m_verbose = verbose;
        m_debug = debug;

        if (m_verbose > 0) {
            System.out.println( "Semantic Check: Attributing parse tree with expression type information");
        }

        if (m_debug) {
            System.out.println( "Semantic Check: Enabled debugging information");
        }

        m_properties = new ParseTreeProperty<TypeDefinition>();
    }

    public TypeDefinition lookupType(ParserRuleContext ctx) {
        return m_properties.get(ctx);
    }

    @Override
    public Void visitBaseExpression(SmallPearlParser.BaseExpressionContext ctx) {
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitBaseExpression");

        if (ctx.primaryExpression() != null) {
            visitPrimaryExpression(ctx.primaryExpression());
            TypeDefinition typeDefinition = m_properties.get(ctx.primaryExpression());
            if( typeDefinition != null) {
                m_properties.put(ctx, typeDefinition);

                if (m_debug) {
                    System.out.println("ExpressionTypeVisitor: visitBaseExpression: exp=" + ctx.primaryExpression().getText());
                    System.out.println("ExpressionTypeVisitor: visitBaseExpression: res=(" + typeDefinition + ")");
                }
            }
        }
        return null;
    }

    @Override
    public Void visitPrimaryExpression(SmallPearlParser.PrimaryExpressionContext ctx) {
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitPrimaryExpression");

        if (ctx.expression() != null) {
            visit(ctx.expression());
            TypeDefinition typeDefinition = m_properties.get(ctx.expression());
            if( typeDefinition != null) {
                m_properties.put(ctx, typeDefinition);
            }
        } else if (ctx.literal() != null) {
            visitLiteral(ctx.literal());
            TypeDefinition typeDefinition = m_properties.get(ctx.literal());
            if( typeDefinition != null) {
                m_properties.put(ctx, typeDefinition);
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.ID() != null) {
            // lookup ID in the symboltable and find out the associated type
            TypeDefinition typeDefinition = new TypeFixed(15);
            m_properties.put(ctx, typeDefinition);
        } else if (ctx.semaTry() != null) {
            visit(ctx.semaTry());
            TypeDefinition typeDefinition = m_properties.get(ctx.semaTry());
            if( typeDefinition != null) {
                m_properties.put(ctx, typeDefinition);
            }
        } else if (ctx.monadicExplicitTypeConversionOperators() != null) {
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitPrimaryExpression:monadicExplicitTypeConversionOperators");

            visit(ctx.monadicExplicitTypeConversionOperators());
            TypeDefinition typeDefinition = m_properties.get(ctx.monadicExplicitTypeConversionOperators());
            if( typeDefinition != null) {
                m_properties.put(ctx, typeDefinition);
            }
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   | Type of   | Result type | Meaning of operation
    //            | operand 1 | operand 2 |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // op1 + op2  | FIXED(g1) | FIXED(g2) | FIXED(g3)   | addition of the values of
    //            | FIXED(g1) | FLOAT(g2) | FLOAT(g3)   |the operands op1 and op2
    //            | FLOAT(g1) | FIXED(g2) | FLOAT(g3)   |
    //            | FLOAT(g1) | FLOAT(g2) | FLOAT(g3)   | g3 = max (g1, g2)
    //            | DURATION  | DURATION  | DURATION    |
    //            | DURATION  | CLOCK     | CLOCK       |
    //            | CLOCK     | DURATION  | CLOCK       |

    @Override
    public Void visitAdditiveExpression(SmallPearlParser.AdditiveExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitAdditiveExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#1");
        }
        else if ( op1 instanceof TypeFixed && op2 instanceof TypeFloat) {
            Integer precision = Math.max( ((TypeFixed) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#2");
        }
        else if ( op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeFloat) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#3");
        }
        else if ( op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max( ((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#4");
        }
        else if ( op1 instanceof TypeDuration && op2 instanceof TypeDuration) {
            res = new TypeDuration(0);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#5");
        }
        else if ( op1 instanceof TypeDuration && op2 instanceof TypeClock) {
            res = new TypeClock(0);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#6");
        }
        else if ( op1 instanceof  TypeClock&& op2 instanceof TypeDuration) {
            res = new TypeClock(0);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#7");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   | Type of   | Result type | Meaning of operation
    //            | operand 1 | operand 2 |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // op1 - op2  | FIXED(g1) | FIXED(g2) | FIXED(g3)   | subtraction of the values of
    //            | FIXED(g1) | FLOAT(g2) | FLOAT(g3)   | the operands op1 and op2
    //            | FLOAT(g1) | FIXED(g2) | FLOAT(g3)   |
    //            | FLOAT(g1) | FLOAT(g2) | FLOAT(g3)   | g3 = max (g1, g2)
    //            | DURATION  | DURATION  | DURATION    |
    //            | CLOCK     | DURATION  | CLOCK       |
    //            | CLOCK     | CLOCK     | DURATION    |

    @Override
    public Void visitSubtractiveExpression(SmallPearlParser.SubtractiveExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSubtractiveExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#1");
        }
        else if ( op1 instanceof TypeFixed && op2 instanceof TypeFloat) {
            Integer precision = Math.max( ((TypeFixed) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#2");
        }
        else if ( op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeFloat) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#3");
        }
        else if ( op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max( ((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#4");
        }
        else if ( op1 instanceof TypeDuration && op2 instanceof TypeDuration) {
            res = new TypeDuration(0);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#5");
        }
        else if ( op1 instanceof TypeDuration && op2 instanceof TypeClock) {
            res = new TypeClock(0);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#6");
        }
        else if ( op1 instanceof  TypeClock&& op2 instanceof TypeDuration) {
            res = new TypeClock(0);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#7");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Monadic operators for numerical, temporal
    //            and bit values
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   |           | Result type | Meaning of operation
    //            | operand   |           |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // + op       | FIXED(g)  |           | FIXED(g)    | identity
    //            | FLOAT(g)  |           | FLOAT(g)    |
    //            | DURATION  |           | DURATION    |

    @Override
    public Void visitUnaryAdditiveExpression(SmallPearlParser.UnaryAdditiveExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitUnaryAdditiveExpression");

        visit(ctx.expression());
        op1 = m_properties.get(ctx.expression());

        if (op1 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op1 instanceof TypeFixed) {
            res = new TypeFixed(((TypeFixed) op1).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnaryAdditiveExpression: rule#1");
        }
        else if ( op1 instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op1).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnaryAdditiveExpression: rule#2");
        }
        else if ( op1 instanceof TypeDuration) {
            res = new TypeDuration(((TypeDuration) op1).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnaryAdditiveExpression: rule#3");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Monadic operators for numerical, temporal
    //            and bit values
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   |           | Result type | Meaning of operation
    //            | operand   |           |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // - op       | FIXED(g)  |           | FIXED(g)    | changing the sign of op
    //            | FLOAT(g)  |           | FLOAT(g)    |
    //            | DURATION  |           | DURATION    |

    @Override
    public Void visitUnarySubtractiveExpression(SmallPearlParser.UnarySubtractiveExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitUnarySubtractiveExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op instanceof TypeFixed) {
            res = new TypeFixed(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnarySubtractiveExpression: rule#1");
        }
        else if ( op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnarySubtractiveExpression: rule#2");
        }
        else if ( op instanceof TypeDuration) {
            res = new TypeDuration(((TypeDuration) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnarySubtractiveExpression: rule#3");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Monadic operators for numerical, temporal
    //           and bit values
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   |           | Result type | Meaning of operation
    //            | operand   |           |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // NOT op     | BIT(lg)   |           | BIT(lg)     | inverting all bit positions of op

    @Override
    public Void visitNotExpression(SmallPearlParser.NotExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitNotExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op instanceof TypeBit) {
            res = new TypeBit(((TypeBit)op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: NotExpression: rule#1");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Monadic operators for numerical, temporal
    //           and bit values
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   |           | Result type | Meaning of operation
    //            | operand   |           |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // ABS op     | FIXED(g)  |           | FIXED(g)    | absolute value of op
    //            | FLOAT(g)  |           | FLOAT(g)    |
    //            | DURATION  |           | DURATION    |

    @Override
    public Void visitAbsExpression(SmallPearlParser.AbsExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitAbsExpressionContext");
        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Monadic operators for numerical, temporal
    //           and bit values
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   |           | Result type | Meaning of operation
    //            | operand   |           |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // SIGN op    | FIXED(g)  |           | FIXED(1)    | determining the sign of op
    //            | FLOAT(g)  |           |             |  1 for op > 0
    //            | DURATION  |           |             |  0 for op = 0
    //            |           |           |             | -1 for op < 0

    @Override
    public Void visitSignExpression(SmallPearlParser.SignExpressionContext ctx) {
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSignExpressionContext");
        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -----------+-----------+-----------+-------------+---------------------------------
    // Expression | Type of   | Type of   | Result type | Meaning of operation
    //            | operand 1 | operand 2 |             |
    // -----------+-----------+-----------+-------------+---------------------------------
    // op1 * op2  | FIXED(g1) | FIXED(g2) | FIXED(g3)   | multiplication of the values of
    //            | FIXED(g1) | FLOAT(g2) | FLOAT(g3)   | the operands op1 and op2
    //            | FLOAT(g1) | FIXED(g2) | FLOAT(g3)   |
    //            | FLOAT(g1) | FLOAT(g2) | FLOAT(g3)   | g3 = max (g1, g2)
    //            | FIXED(g1) | DURATION  | DURATION    |
    //            | DURATION  | FIXED(g2) | DURATION    |
    //            | FLOAT(g1) | DURATION  | DURATION    |
    //            | DURATION  | FLOAT(g2) | DURATION    |
    // -----------+-----------+-----------+-------------+---------------------------------

    @Override
    public Void visitMultiplicativeExpression(SmallPearlParser.MultiplicativeExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#1");
        }
        else if ( op1 instanceof TypeFixed && op2 instanceof TypeFloat) {
            Integer precision = Math.max( ((TypeFixed) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#2");
        }
        else if ( op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeFloat) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#3");
        }
        else if ( op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max( ((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#4");
        }
        else if ( op1 instanceof TypeDuration && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeDuration) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeDuration(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#5");
        }
        else if ( op1 instanceof TypeFloat && op2 instanceof TypeDuration) {
            Integer precision = Math.max( ((TypeFloat) op1).getPrecision(), ((TypeDuration) op2).getPrecision());
            res = new TypeDuration(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#6");
        }
        else if ( op1 instanceof TypeDuration && op2 instanceof TypeFloat) {
            Integer precision = Math.max( ((TypeDuration) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeDuration(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#7");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -------------+-----------+-----------+-------------+---------------------------------
    // Expression   | Type of   | Type of   | Result type | Meaning of operation
    //              | operand 1 | operand 2 |             |
    // -------------+-----------+-----------+-------------+---------------------------------
    // op1 / op2    | FIXED(g1) | FIXED(g2) | FLOAT(g3)   | division of the values of
    //              | FLOAT(g1) | FIXED(g2) | FLOAT(g3)   | the operands op1 and op2,
    //              | FIXED(g1) | FLOAT(g2) | FLOAT(g3)   | if op2 <> 0
    //              | FLOAT(g1) | FLOAT(g2) | FLOAT(g3)   |
    //              | DURATION  | FIXED(g2) | DURATION    | g3 = max (g1, g2)
    //              | DURATION  | FLOAT(g2) | DURATION    | g4 = 31
    //              | DURATION  | DURATION  | FLOAT(g4)   | (dependent on implementation)

    @Override
    public Void visitDivideExpression(SmallPearlParser.DivideExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitDivideExpression");

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -------------+-----------+-----------+-------------+---------------------------------
    // Expression   | Type of   | Type of   | Result type | Meaning of operation
    //              | operand 1 | operand 2 |             |
    // -------------+-----------+-----------+-------------+---------------------------------
    // op1 // op2   | FIXED(g1) | FIXED(g2) | FIXED(g3)   | integer division of the values of
    //              |           |           |             | the operands op1 and op2
    //              |           |           |             | g3 = max (g1, g2)

    @Override
    public Void visitDivideIntegerExpression(SmallPearlParser.DivideIntegerExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitDivideIntegerExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max( ((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideIntegerExpression: rule#1");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -------------+-----------+-----------+-------------+---------------------------------
    // Expression   | Type of   | Type of   | Result type | Meaning of operation
    //              | operand 1 | operand 2 |             |
    // -------------+-----------+-----------+-------------+---------------------------------
    // op1 REM op2  | FIXED(g1) | FIXED(g2) | FIXED(g2)   | remainder of the integer division
    //              | FLOAT(g1) | FLOAT(g2) | FLOAT(g2)   | of the values of the operands op1
    //              |           |           |             | and op2
    // -------------+-----------+-----------+-------------+---------------------------------

    @Override
    public Void visitRemainderExpression(SmallPearlParser.RemainderExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitRemainderExpression");
        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -------------+-----------+-----------+-------------+---------------------------------
    // Expression   | Type of   | Type of   | Result type | Meaning of operation
    //              | operand 1 | operand 2 |             |
    // -------------+-----------+-----------+-------------+---------------------------------
    // op1 ** op2   | FIXED(g1) | FIXED(g2) | FIXED(g2)   | exponentiation of the values
    //              | FLOAT(g1) | FLOAT(g2) | FLOAT(g2)   | of the operands op1 and op2
    // -------------+-----------+-----------+-------------+---------------------------------

    @Override
    public Void visitExponentiationExpression(SmallPearlParser.ExponentiationExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitExponentationExpression");

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //
    // -------------+-----------+-----------+-------------+---------------------------------
    // Expression   | Type of   | Type of   | Result type | Meaning of operation
    //              | operand 1 | operand 2 |             |
    // -------------+-----------+-----------+-------------+---------------------------------
    // op1 FIT op2  | FIXED(g1) | FIXED(g2) | FIXED(g2)   | changing the precision of
    //              | FLOAT(g1) | FLOAT(g2) | FLOAT(g2)   | operand op1 into the precision
    //              |           |           |             | of operand op2
    // -------------+-----------+-----------+-------------+---------------------------------

    @Override
    public Void visitFitExpression(SmallPearlParser.FitExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitFitExpression");
        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //            Table 6.3: Monadic arithmetical operators
    // -----------+-------------+-----------+-------------+---------------------------------
    // Expression | Type of     |           | Result type | Meaning of operation
    //            | operand     |           |             |
    // -----------+-------------+-----------+-------------+---------------------------------
    // SQRT op    | FIXED(g) or |           | FLOAT(g)    | square root of operand
    // SIN op     | FLOAT(g)    |           |             | sine of operand
    // COS op     |             |           |             | cosine of operand
    // EXP op     |             |           |             | e^op with e=2.718281828459
    // LN op      |             |           |             | natural logarithm of operand
    // TAN op     |             |           |             | tangent of operand
    // ATAN op    |             |           |             | arcus tangent of operand
    // TANH op    |             |           |             | tangent hyperbolicus of operand
    // -----------+-------------+-----------+-------------+---------------------------------

    @Override
    public Void visitSqrtExpression(SmallPearlParser.SqrtExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSqrtExpression");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitSinExpression(SmallPearlParser.SinExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSinExpression");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitCosExpression(SmallPearlParser.CosExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitCosExpression");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitExpExpression(SmallPearlParser.ExpExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitExpExpression");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitLnExpression(SmallPearlParser.LnExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitLnExpression");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitTanExpression(SmallPearlParser.TanExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTanExpression");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitAtanExpression(SmallPearlParser.AtanExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitAtanExpression");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitTanhExpression(SmallPearlParser.TanhExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTanhExpression");
        visit(ctx.expression());
        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //            Table 6.2: Monadic operators for explicit type conversions
    // -----------+--------------+-----------+--------------+---------------------------------
    // Expression | Type of      |           | Result type  | Meaning of operation
    //            | operand      |           |              |
    // -----------+------------- +-----------+--------------+---------------------------------
    // TOFIXED op | CHARACTER(1) |           | FIXED(7)     | ASCII code for operand character
    //            | BIT(lg)      |           | FIXED(g)     | interpreting the bit pattern of the
    //            |              |           |              | operand as an integer, with g = lg
    // TOFLOAT op | FIXED(g)     |           | FLOAT(g)     | converting the operand into a
    //            |              |           |              | floating point number
    // TOBIT op   | FIXED(g)     |           | BIT(lg)      | interpreting the operand as bit
    //            |              |           |              | pattern, with lg = g
    // TOCHAR op  | FIXED        |           | CHARRATER(1) | character for the ASCII code of the
    //            |              |           |              | operand
    // ENTIER op  | FLOAT(g)     |           | FIXED(g)     | greatest integer less or equal than
    //            |              |           |              | the operand
    // ROUND op   | FLOAT(g)     |           | FIXED(g)     | next integer according to DIN



    @Override
    public Void visitTOFIXED(SmallPearlParser.TOFIXEDContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTOFIXED");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitTOFLOAT(SmallPearlParser.TOFLOATContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTOFLOAT");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed)op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TOFLOAT: rule#1");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitTOBIT(SmallPearlParser.TOBITContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTOBIT");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null ) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if ( op instanceof TypeFixed) {
            res = new TypeBit(((TypeFixed)op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TOFBIT: rule#1");
        }
        else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitTOCHAR(SmallPearlParser.TOCHARContext ctx) {
        throw new NotYetImplementedException("TOCHAR", ctx.start.getLine(), ctx.start.getCharPositionInLine());    }

    @Override
    public Void visitENTIER(SmallPearlParser.ENTIERContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitENTIER");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitROUND(SmallPearlParser.ROUNDContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitROUND");
        visit(ctx.expression());
        return null;
    }

    @Override
    public Void visitUnaryExpression(SmallPearlParser.UnaryExpressionContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitUnaryExpression");
        throw new NotYetImplementedException("ExpressionTypeVisitor:visitUnaryExpression", ctx.start.getLine(), ctx.start.getCharPositionInLine());

    }

    //
    // Reference: OpenPEARL Language Report
    //            9.3.1 Semaphore Variables (SEMA) and Statements (REQUEST, RELEASE, TRY)
    // -------------+--------------+-----------+--------------+---------------------------------
    // Expression   |              |           | Result type  | Meaning of operation
    //              |              |           |              |
    // -------------+------------- +-----------+--------------+---------------------------------
    // TRY sema     | SEMAPHORE    |           | BIT(1)       | Obtains the state of a semaphore
    //              |              |           |              | variable

    @Override
    public Void visitSemaTry(SmallPearlParser.SemaTryContext ctx) {
        // TODO:
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSemaTryContext");
        throw new NotYetImplementedException("ExpressionTypeVisitor:visitSemaTry", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public Void visitLiteral(SmallPearlParser.LiteralContext ctx) {
        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitLiteral");

        if (ctx.IntegerConstant() != null) {
            try {
                Integer value = Integer.parseInt(ctx.IntegerConstant().toString());
                Integer precision = Integer.toBinaryString(Math.abs(value)).length();
                m_properties.put(ctx, new TypeFixed(precision));
            }
            catch(NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.StringLiteral() != null) {
            m_properties.put(ctx, new TypeChar());
        } else if (ctx.FloatingPointConstant() != null) {
            try {
                Float value = Float.parseFloat(ctx.FloatingPointConstant().toString());
                Integer precision = 24;
                m_properties.put(ctx, new TypeFloat(precision));
            }
            catch(NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

        } else if (ctx.timeConstant() != null) {
            // TODO:
        } else if (ctx.durationConstant() != null) {
            // TODO:
        }

        return null;
    }
}