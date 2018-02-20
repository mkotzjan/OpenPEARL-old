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
import org.smallpearl.compiler.SymbolTable.ModuleEntry;
import org.smallpearl.compiler.SymbolTable.SymbolTable;
import org.smallpearl.compiler.SymbolTable.SymbolTableEntry;
import org.smallpearl.compiler.SymbolTable.VariableEntry;


public class ConstantPoolVisitor extends SmallPearlBaseVisitor<Void> implements SmallPearlVisitor<Void> {
    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;
    private SymbolTableVisitor m_symbolTableVisitor;
    private SymbolTable m_symboltable;
    private SymbolTable m_currentSymbolTable;
    private ModuleEntry m_module;
    private int m_counter = 0;
    private ConstantPool m_constantPool = null;
    private ParseTreeProperty<TypeDefinition> m_properties = null;
    private Integer m_currFixedLength = null;
    private ExpressionTypeVisitor m_expressionTypeVisitor = null;

    public ConstantPoolVisitor( String sourceFileName,
                                int verbose,
                                boolean debug,
                                SymbolTableVisitor symbolTableVisitor,
                                ConstantPool constantPool,
                                ExpressionTypeVisitor expressionTypeVisitor) {


        m_debug = debug;
        m_verbose = verbose;
        m_sourceFileName = sourceFileName;
        m_symbolTableVisitor = symbolTableVisitor;
        m_symboltable = symbolTableVisitor.symbolTable;
        m_currentSymbolTable = m_symboltable;
        m_constantPool = constantPool;
        m_properties = new ParseTreeProperty<TypeDefinition>();
        m_expressionTypeVisitor = expressionTypeVisitor;

        if (m_verbose > 0) {
            System.out.println( "Start ConstantPoolVisitor");
        }

        // Add commonly used constants:
        m_constantPool.add(new ConstantFixedValue(0,Defaults.FIXED_LENGTH));
        m_constantPool.add(new ConstantFixedValue(1,Defaults.FIXED_LENGTH));
        m_constantPool.add(new ConstantFixedValue(-1,Defaults.FIXED_LENGTH));
    }

    public Void add(ConstantValue value) {
        int i;
        int constantBitNo = 0;

        boolean found = false;

        for (i = 0; i < m_constantPool.constantPool.size(); i++) {
            if ( value instanceof ConstantFixedValue) {
                if (m_constantPool.constantPool.get(i) instanceof ConstantFixedValue) {
                    ConstantFixedValue a = (ConstantFixedValue)(value);
                    ConstantFixedValue b = (ConstantFixedValue)(m_constantPool.constantPool.get(i));

                    if ( a.getValue() == b.getValue() && a.getPrecision() == b.getPrecision()) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantFloatValue) {
                if (m_constantPool.constantPool.get(i) instanceof ConstantFloatValue) {
                    if ( Double.compare( ((ConstantFloatValue)(value)).getValue(), ((ConstantFloatValue)(m_constantPool.constantPool.get(i))).getValue()) == 0) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantCharacterValue) {
                if (m_constantPool.constantPool.get(i) instanceof ConstantCharacterValue) {
                    String s1 = ((ConstantCharacterValue)(value)).getValue();
                    String s2 = ((ConstantCharacterValue)(m_constantPool.constantPool.get(i))).getValue();

                    if ( s1.equals(s2)) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantBitValue) {
                if (m_constantPool.constantPool.get(i) instanceof ConstantBitValue) {
                    constantBitNo = constantBitNo + 1;
                    String s1 = ((ConstantBitValue)(value)).getValue();
                    String s2 = ((ConstantBitValue)(m_constantPool.constantPool.get(i))).getValue();

                    if ( s1.equals(s2)) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantDurationValue) {
                if (m_constantPool.constantPool.get(i) instanceof ConstantDurationValue) {
                    ConstantDurationValue other = (ConstantDurationValue)value;
                    ConstantDurationValue constant = ((ConstantDurationValue)(m_constantPool.constantPool.get(i)));

                    if ( constant.equals(other)) {
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

            m_constantPool.add(value);
        }

        return null;

    }

    @Override
    public Void visitLiteral(SmallPearlParser.LiteralContext ctx) {
        if (m_debug)
            System.out.println("ConstantPoolVisitor: visitLiteral");

        if (ctx.durationConstant() != null) {
            int hours = 0;
            int minutes = 0;
            double seconds = 0.0;

            if ( ctx.durationConstant().hours() != null ) {
                hours = getHours(ctx.durationConstant().hours());
            }

            if ( ctx.durationConstant().minutes() != null ) {
                minutes = getMinutes(ctx.durationConstant().minutes());
            }

            if ( ctx.durationConstant().seconds() !=  null) {
            seconds = getSeconds(ctx.durationConstant().seconds());
            }

            m_constantPool.add(new ConstantDurationValue(hours, minutes, seconds));
        } else if (ctx.timeConstant() != null) {
            int hours = 0;
            int minutes = 0;
            double seconds = 0.0;

            hours = Integer.valueOf(ctx.timeConstant().IntegerConstant(0).toString());
            minutes = Integer.valueOf(ctx.timeConstant().IntegerConstant(1).toString());

            if (ctx.timeConstant().IntegerConstant(3) != null) {
                seconds = Double.valueOf(ctx.timeConstant().IntegerConstant(3).toString());
            } else if (ctx.timeConstant().floatingPointConstant() != null) {
                seconds = Double.valueOf(ctx.timeConstant().floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
            }

            m_constantPool.add(new ConstantClockValue(hours, minutes, seconds));
        } else if (ctx.BitStringLiteral() != null) {
            long value = CommonUtils.convertBitStringToLong(ctx.BitStringLiteral().getText());
            int  nb = CommonUtils.getBitStringLength(ctx.BitStringLiteral().getText());
            m_constantPool.add(new ConstantBitValue(value,nb));
        } else if (ctx.floatingPointConstant() != null) {
            try {
                double value = 0.0;
                int precision = m_currentSymbolTable.lookupDefaultFloatLength();

                if ( ctx.floatingPointConstant().FloatingPointNumberPrecision() != null) {
                    precision = Integer.valueOf(ctx.floatingPointConstant().FloatingPointNumberPrecision().toString());
                }

                value = Double.valueOf(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());

                m_constantPool.add(new ConstantFloatValue(value,precision));

            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

        } else if (ctx.StringLiteral() != null) {
            String s = ctx.StringLiteral().toString();
            add(new ConstantCharacterValue(s));
        } else if (ctx.fixedConstant() != null) {
            try {
                int value;
                int precision = m_currentSymbolTable.lookupDefaultFixedLength();

                if ( ctx.fixedConstant().fixedNumberPrecision() != null) {
                    precision = Integer.parseInt(ctx.fixedConstant().fixedNumberPrecision().IntegerConstant().toString());
                } else {
                    if (m_currFixedLength != null ) {
                        precision = m_currFixedLength;
                    }
                }

                value = Integer.parseInt(ctx.fixedConstant().IntegerConstant().toString());

                m_constantPool.add(new ConstantFixedValue(value,precision));
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

                    if (literal_ctx.floatingPointConstant() != null) {
                        try {
                            double value = -1 * Double.parseDouble(literal_ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
                            int precision = m_currentSymbolTable.lookupDefaultFloatLength();
                            add(new ConstantFloatValue(value,precision));
                        } catch (NumberFormatException ex) {
                            throw new NumberOutOfRangeException(ctx.getText(), literal_ctx.start.getLine(), literal_ctx.start.getCharPositionInLine());
                        }
                    }
                    else if (literal_ctx.fixedConstant() != null) {
                        try {
                            Integer value = null;
                            int precision = m_currentSymbolTable.lookupDefaultFixedLength();

                            if ( literal_ctx.fixedConstant().fixedNumberPrecision() != null) {
                                precision = Integer.parseInt(literal_ctx.fixedConstant().fixedNumberPrecision().IntegerConstant().toString());
                            }

                            value = -1 * Integer.parseInt(literal_ctx.fixedConstant().IntegerConstant().toString());

                            add(new ConstantFixedValue(value, precision));
                        } catch (NumberFormatException ex) {
                            throw new NumberOutOfRangeException(ctx.getText(), literal_ctx.start.getLine(), literal_ctx.start.getCharPositionInLine());
                        }
                    } else if (literal_ctx.StringLiteral() != null) {
                        System.out.println("string:(" + literal_ctx.StringLiteral().toString() + ")");
                    } else if (literal_ctx.floatingPointConstant() != null) {
                        try {
                            double value = -1 * Double.parseDouble(literal_ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
                            int precision = m_currentSymbolTable.lookupDefaultFloatLength();
                            add(new ConstantFloatValue(value,precision));
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

/*
    @Override
    public Void visitConstant(SmallPearlParser.ConstantContext ctx) {
        if (m_debug)
            System.out.println("ConstantPoolVisitor: visitConstant");

        if ( ctx.fixedConstant() != null ) {
            try {
                int value;
                int precision = m_currentSymbolTable.lookupDefaultFixedLength();

                value = Integer.parseInt(ctx.fixedConstant().IntegerConstant().toString());

                if (ctx.sign() != null) {
                    if (ctx.sign() instanceof SmallPearlParser.SignMinusContext) {
                        value = -1 * value;
                    }
                }

                if (ctx.fixedConstant().fixedNumberPrecision() != null ) {
                    precision = Integer.parseInt(ctx.fixedConstant().fixedNumberPrecision().IntegerConstant().toString());
                }
                else {
                    // walk up the AST and get VariableDenotationContext:
                    ParserRuleContext sctx = ctx.getParent();
                    while (sctx != null && !(sctx instanceof SmallPearlParser.VariableDenotationContext)) {
                        sctx = sctx.getParent();
                    }

                    if (sctx != null) {
                        SmallPearlParser.TypeAttributeContext typeAttributeContext = ((SmallPearlParser.VariableDenotationContext) sctx).typeAttribute();
                        if (typeAttributeContext.simpleType() != null) {
                            SmallPearlParser.SimpleTypeContext simpleTypeContext = typeAttributeContext.simpleType();

                            if (simpleTypeContext.typeInteger() != null) {
                                SmallPearlParser.TypeIntegerContext typeIntegerContext = simpleTypeContext.typeInteger();

                                if (typeIntegerContext.mprecision() != null) {
                                    precision = Integer.parseInt(typeIntegerContext.mprecision().integerWithoutPrecision().IntegerConstant().toString());
                                }
                            }
                        }

                    } else {
                        throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                    }
                }

                value = Integer.parseInt(ctx.fixedConstant().IntegerConstant().toString());

                if (ctx.sign() != null) {
                    if (ctx.sign() instanceof SmallPearlParser.SignMinusContext) {
                        value = -1 * value;
                    }
                }

                m_constantPool.add(new ConstantFixedValue(value, precision));
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
        else if ( ctx.StringLiteral() != null ) {
            m_constantPool.add(new ConstantCharacterValue(ctx.StringLiteral().toString()));
        }
        else if ( ctx.durationConstant() != null ) {
            Integer hours = 0;
            Integer minutes = 0;
            Double seconds = 0.0;

            if (ctx.durationConstant().hours() != null) {
                hours = Integer.valueOf(ctx.durationConstant().hours().IntegerConstant().toString());
            }
            if (ctx.durationConstant().minutes() != null) {
                minutes = Integer.valueOf(ctx.durationConstant().minutes().IntegerConstant().toString());
            }
            if (ctx.durationConstant().seconds() != null) {
                if (ctx.durationConstant().seconds().IntegerConstant() != null) {
                    seconds = Double.valueOf(ctx.durationConstant().seconds().IntegerConstant().toString());
                } else if (ctx.durationConstant().seconds().floatingPointConstant() != null) {
                    seconds = Double.valueOf(ctx.durationConstant().seconds().floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
                }
            }

            ConstantDurationValue durationConst = new ConstantDurationValue(hours, minutes, seconds);
            m_constantPool.add(durationConst);
        }
        else if ( ctx.timeConstant() != null ) {
            Integer hours = 0;
            Integer minutes = 0;
            Double seconds = 0.0;

            hours = Integer.valueOf(ctx.timeConstant().IntegerConstant(0).toString());
            minutes = Integer.valueOf(ctx.timeConstant().IntegerConstant(1).toString());

            if (ctx.timeConstant().floatingPointConstant() != null) {
                seconds = Double.valueOf(ctx.timeConstant().floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());
            }
            else if (ctx.timeConstant().IntegerConstant(2) != null) {
                seconds = Double.valueOf(ctx.timeConstant().IntegerConstant(2).toString());
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            ConstantClockValue clockConst = new ConstantClockValue(hours, minutes, seconds);
            m_constantPool.add(clockConst);
        }

        else if ( ctx.floatingPointConstant() != null ) {
            try {
                double  value = 0.0;
                int precision = m_currentSymbolTable.lookupDefaultFloatLength();

                // walk up the AST and get VariableDenotationContext:
                ParserRuleContext  sctx =  ctx.getParent();
                while ( sctx != null && !(sctx instanceof SmallPearlParser.VariableDenotationContext)) {
                    sctx = sctx.getParent();
                }

                if (sctx != null) {
                    SmallPearlParser.TypeAttributeContext typeAttributeContext = ((SmallPearlParser.VariableDenotationContext)sctx).typeAttribute();
                    if ( typeAttributeContext.simpleType() != null ) {
                        SmallPearlParser.SimpleTypeContext simpleTypeContext = typeAttributeContext.simpleType();

                        if( simpleTypeContext.typeFloatingPointNumber() != null ) {
                            SmallPearlParser.TypeFloatingPointNumberContext typeFloatingPointNumberContext = simpleTypeContext.typeFloatingPointNumber();

                            if ( typeFloatingPointNumberContext.IntegerConstant() != null) {
                                precision = Integer.valueOf(typeFloatingPointNumberContext.IntegerConstant().toString());
                            }
                        }
                    }

                }
                else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }

                value = Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().toString());

                if ( ctx.sign() != null ) {
                    if ( ctx.sign() instanceof SmallPearlParser.SignMinusContext) {
                        value = -1 * value;
                    }
                }

                m_constantPool.add(new ConstantFloatValue(value,precision));
            } catch (NumberFormatException ex) {
                throw new NumberOutOfRangeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
        else if ( ctx.bitStringConstant() != null ) {
            long value = CommonUtils.convertBitStringToLong(ctx.bitStringConstant().BitStringLiteral().getText());
            int  nb = CommonUtils.getBitStringLength(ctx.bitStringConstant().BitStringLiteral().getText());
            ConstantBitValue cbv = new ConstantBitValue(value,nb);
            m_constantPool.add(cbv);
        }

        return null;
    }
*/

    private Integer getHours(SmallPearlParser.HoursContext ctx) {
        Integer hours = 0;

        if (ctx.IntegerConstant() != null) {
            hours = Integer.parseInt(ctx.IntegerConstant().getText());
            if (hours < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return hours;
    }

    private Integer getMinutes(SmallPearlParser.MinutesContext ctx) {
        Integer minutes = 0;

        if (ctx.IntegerConstant() != null) {
            minutes = Integer.parseInt(ctx.IntegerConstant().getText());
            if (minutes < 0 || minutes > 59) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return minutes;
    }

    private Double getSeconds(SmallPearlParser.SecondsContext ctx) {
        Double seconds = 0.0;

        if (ctx.IntegerConstant() != null) {
            seconds = (double) Integer.parseInt(ctx.IntegerConstant().getText());
            if (seconds < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.floatingPointConstant() != null) {
            Integer sign = 1;

            seconds = Double.parseDouble(ctx.floatingPointConstant().FloatingPointNumberWithoutPrecision().getText());

            if (seconds < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return seconds;
    }
    @Override
    public Void visitModule(SmallPearlParser.ModuleContext ctx) {
        if (m_debug) {
            System.out.println( "ConstantPoolVisitor: visitModule");
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
            System.out.println( "ConstantPoolVisitor: visitProcedureDeclaration");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        if (m_debug) {
            System.out.println( "ConstantPoolVisitor: visitTaskDeclaration");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        m_currentSymbolTable = m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        if (m_debug) {
            System.out.println( "ConstantPoolVisitor: visitBlock_statement");
        }

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);
        visitChildren(ctx);
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitLoopStatement(SmallPearlParser.LoopStatementContext ctx) {
        if (m_debug) {
            System.out.println( "ConstantPoolVisitor: visitLoopStatement");
        }

        int precision;

        this.m_currentSymbolTable = m_symbolTableVisitor.getSymbolTablePerContext(ctx);


        if ( ctx.loopStatement_for() == null ) {
            visitChildren(ctx);
        }
        else {
            SymbolTableEntry entry = m_currentSymbolTable.lookup(ctx.loopStatement_for().ID().getText());
            VariableEntry var = null;

            if ( entry != null && entry instanceof VariableEntry) {
                var = (VariableEntry) entry;
            }
            else {
                throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
            TypeFixed fromType = null;
            TypeFixed toType = null;

            ExpressionResult fromRes = null;
            ExpressionResult toRes = null;
            if ( ctx.loopStatement_from() != null ) {
                fromRes  = m_expressionTypeVisitor.lookup(ctx.loopStatement_to().expression());
                TypeDefinition typ = var.getType();
                if ( typ instanceof TypeFixed) {
                    fromType = (TypeFixed)typ;
                }
                else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }

            if ( ctx.loopStatement_to() != null ) {
                toRes  = m_expressionTypeVisitor.lookup(ctx.loopStatement_to().expression());
                TypeDefinition typ = var.getType();
                if ( typ instanceof TypeFixed) {
                    toType = (TypeFixed)typ; //TODO stimmt nicht!!!!
                }
                else {
                    throw new InternalCompilerErrorException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }

            precision = m_currentSymbolTable.lookupDefaultFixedLength();

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

//TODO            m_constantPool.add(new ConstantFixedValue(value,precision));
        }
        this.m_currentSymbolTable = this.m_currentSymbolTable.ascend();
        return null;
    }

    @Override
    public Void visitAssignment_statement(SmallPearlParser.Assignment_statementContext ctx) {
        String id = null;

        if (m_verbose > 0) {
            System.out.println("ConstantPoolVisitor: visitAssignment_statement");
        }

        if ( ctx.stringSelection() != null ) {
            if ( ctx.stringSelection().charSelection() != null ) {
                id = ctx.stringSelection().charSelection().ID().getText();
            }
        } else {
            id = ctx.ID().getText();
        }

        SymbolTableEntry entry = m_currentSymbolTable.lookup(id);

        if (!(entry instanceof VariableEntry)) {
            throw  new UnknownIdentifierException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        SmallPearlParser.ExpressionContext expr = ctx.expression();

        VariableEntry var = (VariableEntry)entry;
        if ( var.getType() instanceof TypeFixed) {
            TypeFixed typ = (TypeFixed)(var.getType());
            m_currFixedLength = typ.getPrecision();
        }

        visitChildren(ctx);

        m_currFixedLength = null;
        return null;
    }

}
