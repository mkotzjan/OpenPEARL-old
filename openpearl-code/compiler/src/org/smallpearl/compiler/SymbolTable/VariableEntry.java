package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.SmallPearlParser;
import org.smallpearl.compiler.TypeDefinition;

public class VariableEntry extends SymbolTableEntry {

    private TypeDefinition  m_type;
    private Boolean m_hasAssigmentProtection;

    public VariableEntry() {
    }

    public VariableEntry(String name, TypeDefinition type, org.antlr.v4.runtime.ParserRuleContext ctx)
    {
        super(name);
        this.m_ctx = ctx;
        this.m_type = type;
        this.m_hasAssigmentProtection = false;
    }

    public VariableEntry(String name, TypeDefinition type, Boolean hasAssigmentProtection, SmallPearlParser.VariableDenotationContext ctx)
    {
        super(name);
        this.m_ctx = ctx;
        this.m_type = type;
        this.m_hasAssigmentProtection = hasAssigmentProtection;
    }

    public String toString(int level) {
        String assigmenProtection = this.m_hasAssigmentProtection ? "INV" : "";
        return indentString(level) + super.toString(level) + "var " + m_type + " " + assigmenProtection;
    }

    public int getSourceLineNo() {
        return m_ctx.getStart().getLine();
    }
    public int getCharPositionInLine() {
        return m_ctx.getStart().getCharPositionInLine();
    }
    private org.antlr.v4.runtime.ParserRuleContext m_ctx;
    public TypeDefinition getType() { return m_type; }
    public Boolean getAssigmentProtection() { return m_hasAssigmentProtection; }
}