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

import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.smallpearl.compiler.SymbolTable.*;

import java.util.LinkedList;

public  class ExpressionTypeVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {

    private int m_verbose;
    private boolean m_debug;
    private SymbolTableVisitor m_symbolTableVisitor;
    private org.smallpearl.compiler.SymbolTable.SymbolTable m_symboltable;
    private org.smallpearl.compiler.SymbolTable.SymbolTable m_currentSymbolTable;
    private org.smallpearl.compiler.SymbolTable.ModuleEntry m_module;
    private ParseTreeProperty<TypeDefinition> m_properties = null;

    public ExpressionTypeVisitor(int verbose, boolean debug, SymbolTableVisitor symbolTableVisitor) {

        m_verbose = verbose;
        m_debug = debug;

        m_symbolTableVisitor = symbolTableVisitor;
        m_symboltable = symbolTableVisitor.symbolTable;

        if (m_verbose > 0) {
            System.out.println("Semantic Check: Attributing parse tree with expression type information");
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
            if (typeDefinition != null) {
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
            if (typeDefinition != null) {
                m_properties.put(ctx, typeDefinition);
            }
        } else if (ctx.literal() != null) {
            visitLiteral(ctx.literal());
            TypeDefinition typeDefinition = m_properties.get(ctx.literal());
            if (typeDefinition != null) {
                m_properties.put(ctx, typeDefinition);
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.ID() != null) {
            SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.ID().getText());
            if (entry instanceof VariableEntry) {
                m_properties.put(ctx, ((VariableEntry) entry).getType());
            } else if (entry instanceof ProcedureEntry) {
                ProcedureEntry proc = (ProcedureEntry) entry;
                TypeDefinition resultType = proc.getResultType();
                if (resultType != null) {
                    m_properties.put(ctx, resultType);
                } else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            } else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.semaTry() != null) {
            visit(ctx.semaTry());
            TypeDefinition typeDefinition = m_properties.get(ctx.semaTry());
            if (typeDefinition != null) {
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

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#1");
        } else if (op1 instanceof TypeFixed && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#2");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#3");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#4");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeDuration) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#5");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeClock) {
            res = new TypeClock();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#6");
        } else if (op1 instanceof TypeClock && op2 instanceof TypeDuration) {
            res = new TypeClock();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AdditiveExpression: rule#7");
        } else {
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

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#1");
        } else if (op1 instanceof TypeFixed && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#2");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#3");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#4");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeDuration) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#5");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeClock) {
            res = new TypeClock();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#6");
        } else if (op1 instanceof TypeClock && op2 instanceof TypeDuration) {
            res = new TypeClock();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SubtractiveExpression: rule#7");
        } else {
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

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed) {
            res = new TypeFixed(((TypeFixed) op1).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnaryAdditiveExpression: rule#1");
        } else if (op1 instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op1).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnaryAdditiveExpression: rule#2");
        } else if (op1 instanceof TypeDuration) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnaryAdditiveExpression: rule#3");
        } else {
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

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFixed(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnarySubtractiveExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnarySubtractiveExpression: rule#2");
        } else if (op instanceof TypeDuration) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: UnarySubtractiveExpression: rule#3");
        } else {
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

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeBit) {
            res = new TypeBit(((TypeBit) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: NotExpression: rule#1");
        } else {
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
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitAbsExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFixed(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AbsExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AbsExpression: rule#2");
        } else if (op instanceof TypeDuration) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: AbsExpression: rule#3");
        } else {
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
    // SIGN op    | FIXED(g)  |           | FIXED(1)    | determining the sign of op
    //            | FLOAT(g)  |           |             |  1 for op > 0
    //            | DURATION  |           |             |  0 for op = 0
    //            |           |           |             | -1 for op < 0

    @Override
    public Void visitSignExpression(SmallPearlParser.SignExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSignExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFixed(1);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SignExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFixed(1);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SignExpression: rule#2");
        } else if (op instanceof TypeDuration) {
            res = new TypeFixed(1);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SignExpression: rule#3");
        } else {
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

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#1");
        } else if (op1 instanceof TypeFixed && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#2");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#3");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#4");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeFixed) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#5");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeDuration) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#6");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeFloat) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitMultiplicativeExpression: rule#7");
        } else {
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
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitDivideExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#1");
        } else if (op1 instanceof TypeFixed && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#2");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#3");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#4");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeFixed) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#5");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeDuration) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#6");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeFloat) {
            res = new TypeDuration();
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#7");
        } else if (op1 instanceof TypeDuration && op2 instanceof TypeDuration) {
            res = new TypeFloat(53);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideExpression: rule#7");
        } else {
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

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: DivideIntegerExpression: rule#1");
        } else {
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
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitRemainderExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitRemainderExpression: rule#1");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = Math.max(((TypeFloat) op1).getPrecision(), ((TypeFloat) op2).getPrecision());
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: visitRemainderExpression: rule#1");
        } else {
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
    // op1 ** op2   | FIXED(g1) | FIXED(g2) | FIXED(g2)   | exponentiation of the values
    //              | FLOAT(g1) | FLOAT(g2) | FLOAT(g2)   | of the operands op1 and op2
    // -------------+-----------+-----------+-------------+---------------------------------

    @Override
    public Void visitExponentiationExpression(SmallPearlParser.ExponentiationExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitExponentiationExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = Math.max(((TypeFixed) op1).getPrecision(), ((TypeFixed) op2).getPrecision());
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ExponentiationExpression: rule#1");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFixed) {
            res = new TypeFloat(((TypeFloat) op1).getPrecision());
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ExponentiationExpression: rule#1");
        } else {
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
    // op1 FIT op2  | FIXED(g1) | FIXED(g2) | FIXED(g2)   | changing the precision of
    //              | FLOAT(g1) | FLOAT(g2) | FLOAT(g2)   | operand op1 into the precision
    //              |           |           |             | of operand op2
    // -------------+-----------+-----------+-------------+---------------------------------

    @Override
    public Void visitFitExpression(SmallPearlParser.FitExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitFitExpression");

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeFixed && op2 instanceof TypeFixed) {
            Integer precision = ((TypeFixed) op2).getPrecision();
            res = new TypeFixed(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: FitExpression: rule#1");
        } else if (op1 instanceof TypeFloat && op2 instanceof TypeFloat) {
            Integer precision = ((TypeFloat) op2).getPrecision();
            res = new TypeFloat(precision);
            m_properties.put(ctx, res);

            if (m_debug)
                System.out.println("ExpressionTypeVisitor: FitExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

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
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSqrtExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SqrtExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SqrtExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitSinExpression(SmallPearlParser.SinExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSinExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SinExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: SinExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitCosExpression(SmallPearlParser.CosExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitCosExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: CosExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: CosExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitExpExpression(SmallPearlParser.ExpExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitExpExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ExpExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ExpExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitLnExpression(SmallPearlParser.LnExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitLnExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: LnExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: LnExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitTanExpression(SmallPearlParser.TanExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTanExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TanExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TanExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitAtanExpression(SmallPearlParser.AtanExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitATanExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ATanExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ATanExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitTanhExpression(SmallPearlParser.TanhExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTanhExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TanhExpression: rule#1");
        } else if (op instanceof TypeFloat) {
            res = new TypeFloat(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TanhExpression: rule#2");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

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
    public Void visitTOFIXEDExpression(SmallPearlParser.TOFIXEDExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTOFIXEDExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeBit) {
            res = new TypeFixed(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TOFLOAT: rule#1");
        } else if (op instanceof TypeChar) {
            throw new NotYetImplementedException("CHARACTER", ctx.start.getLine(), ctx.start.getCharPositionInLine());
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitTOFLOATExpression(SmallPearlParser.TOFLOATExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTOFLOATExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeFloat(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TOFLOAT: rule#1");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitTOBITExpression(SmallPearlParser.TOBITExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitTOBITExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFixed) {
            res = new TypeBit(((TypeFixed) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: TOBIT: rule#1");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitTOCHARExpression(SmallPearlParser.TOCHARExpressionContext ctx) {
        throw new NotYetImplementedException("TOCHAR", ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //            Table 6.2: Monadic operators for explicit type conversions
    // -----------+--------------+-----------+--------------+---------------------------------
    // Expression | Type of      |           | Result type  | Meaning of operation
    //            | operand      |           |              |
    // -----------+------------- +-----------+--------------+---------------------------------
    // ENTIER op  | FLOAT(g)     |           | FIXED(g)     | greatest integer less or equal than

    @Override
    public Void visitEntierExpression(SmallPearlParser.EntierExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitEntierExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFloat) {
            res = new TypeFixed(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ENTIER: rule#1");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    //
    // Reference: OpenPEARL Language Report 6.1 Expressions
    //            Table 6.2: Monadic operators for explicit type conversions
    // -----------+--------------+-----------+--------------+---------------------------------
    // Expression | Type of      |           | Result type  | Meaning of operation
    //            | operand      |           |              |
    // -----------+------------- +-----------+--------------+---------------------------------
    // ROUND op   | FLOAT(g)     |           | FIXED(g)     | next integer according to DIN

    @Override
    public Void visitRoundExpression(SmallPearlParser.RoundExpressionContext ctx) {
        TypeDefinition op;
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitRoundExpression");

        visit(ctx.expression());
        op = m_properties.get(ctx.expression());

        if (op == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op instanceof TypeFloat) {
            res = new TypeFixed(((TypeFloat) op).getPrecision());
            m_properties.put(ctx, res);
            if (m_debug)
                System.out.println("ExpressionTypeVisitor: ROUND: rule#1");
        } else {
            throw new IllegalExpressionException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

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
        TypeDefinition res;

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: visitSemaTry");

        if (ctx.ID() == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        res = new TypeBit(1);
        m_properties.put(ctx, res);

        if (m_debug)
            System.out.println("ExpressionTypeVisitor: TRY: rule#1");

        return null;
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
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.StringLiteral() != null) {
            m_properties.put(ctx, new TypeChar());
        } else if (ctx.FloatingPointConstant() != null) {
            try {
                Float value = Float.parseFloat(ctx.FloatingPointConstant().toString());
                Integer precision = 24;
                m_properties.put(ctx, new TypeFloat(precision));
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

        } else if (ctx.timeConstant() != null) {
            m_properties.put(ctx, new TypeClock(getTime(ctx.timeConstant())));

        } else if (ctx.durationConstant() != null) {
            m_properties.put(ctx, new TypeDuration());
        } else if (ctx.BitStringLiteral() != null) {
            m_properties.put(ctx, new TypeBit());
        }

        return null;
    }

    private Double getTime(SmallPearlParser.TimeConstantContext ctx) {
        Integer hours = 0;
        Integer minutes = 0;
        Double seconds = 0.0;

        hours = (Integer.valueOf(ctx.getChild(0).toString()) % 24);
        minutes = Integer.valueOf(ctx.getChild(2).toString());
        seconds = Double.valueOf(ctx.getChild(4).toString());

        if (hours < 0 || minutes < 0 || minutes > 59) {
            throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return hours * 3600 + minutes * 60 + seconds;
    }


    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        if (m_verbose > 0) {
            System.out.println("SymbolTableVisitor: visitModule");
        }

//        this.m_currentSymbolTable = this.symbolTable.newLevel(moduleEntry);

        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }


    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: vistTaskDeclaration");
        }

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
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitProcedureDeclaration");
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
            System.out.println("ExpressionTypeVisitor: visitBlock_statement");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitLoopStatement");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitAssignment_statement(SmallPearlParser.Assignment_statementContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitAssignment_statement");
        }

        SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.ID().getText());
        if (!(entry instanceof VariableEntry)) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        visitChildren(ctx);
        return null;
    }

    @Override
    public Void visitNowFunction(SmallPearlParser.NowFunctionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitNowFunction");
        }

        m_properties.put(ctx, new TypeClock());
        return null;
    }

    @Override
    public Void visitSizeofExpression(SmallPearlParser.SizeofExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitSizeofExpression");
        }

        m_properties.put(ctx, new TypeFixed(31));
        return null;
    }

    @Override
    public Void visitLtRelationalExpression(SmallPearlParser.LtRelationalExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitLtRelationalExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }

    @Override
    public Void visitEqRelationalExpression(SmallPearlParser.EqRelationalExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitEqRelationalExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }

    @Override
    public Void visitNeRelationalExpression(SmallPearlParser.NeRelationalExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitNeRelationalExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }


    @Override
    public Void visitLeRelationalExpression(SmallPearlParser.LeRelationalExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitLeRelationalExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }

    @Override
    public Void visitGtRelationalExpression(SmallPearlParser.GtRelationalExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitGtRelationalExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }

    @Override
    public Void visitGeRelationalExpression(SmallPearlParser.GeRelationalExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitGeRelationalExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }

    @Override
    public Void visitConditionalAndExpression(SmallPearlParser.ConditionalAndExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitConditionalAndExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }

    @Override
    public Void visitConditionalOrExpression(SmallPearlParser.ConditionalOrExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitConditionalOrExpression");
        }

        m_properties.put(ctx, new TypeBit(1));

        return null;
    }

    @Override
    public Void visitStringSelection(SmallPearlParser.StringSelectionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitStringSelection");
        }

//        m_properties.put(ctx, new TypeBit(1));
        throw new NotSupportedFeatureException("visitStringSelection", ctx.start.getLine(), ctx.start.getCharPositionInLine(), "???");

        //return null;
    }

    @Override
    public Void visitCshiftExpression(SmallPearlParser.CshiftExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;

        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitCshiftExpression");
        }

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeBit && op2 instanceof TypeFixed) {
            m_properties.put(ctx, new TypeBit(((TypeBit)op1).getPrecision()));

            if (m_debug) {
                System.out.println("ExpressionTypeVisitor: Dyadic Boolean and shift operators");
            }
        }
        else {
            throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }


    @Override
    public Void visitShiftExpression(SmallPearlParser.ShiftExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;

        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitShiftExpression");
        }

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeBit && op2 instanceof TypeFixed) {
            m_properties.put(ctx, new TypeBit(((TypeBit)op1).getPrecision()));

            if (m_debug) {
                System.out.println("ExpressionTypeVisitor: Dyadic Boolean and shift operators");
            }
        }
        else {
            throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitCatExpression(SmallPearlParser.CatExpressionContext ctx) {
        TypeDefinition op1;
        TypeDefinition op2;

        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitCatExpression");
        }

        visit(ctx.expression(0));
        op1 = m_properties.get(ctx.expression(0));

        if (op1 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }
        visit(ctx.expression(1));
        op2 = m_properties.get(ctx.expression(1));

        if (op2 == null) {
            throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        if (op1 instanceof TypeBit && op2 instanceof TypeBit) {
            m_properties.put(ctx, new TypeBit(((TypeBit)op1).getPrecision() + ((TypeBit)op2).getPrecision()));

            if (m_debug) {
                System.out.println("ExpressionTypeVisitor: Dyadic Boolean and shift operators");
            }
        }
        else {
            throw new TypeMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return null;
    }

    @Override
    public Void visitAndExpression(SmallPearlParser.AndExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitAndExpression");
        }

        throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public Void visitOrExpression(SmallPearlParser.OrExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitOrExpression");
        }

        throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

    @Override
    public Void visitExorExpression(SmallPearlParser.ExorExpressionContext ctx) {
        if (m_verbose > 0) {
            System.out.println("ExpressionTypeVisitor: visitExorExpression");
        }

        throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
    }

}