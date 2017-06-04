package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.SmallPearlParser;
import org.smallpearl.compiler.TypeDefinition;

public class VariableEntry extends SymbolTableEntry {

    private TypeDefinition  m_type;
    private Boolean         m_hasAssigmentProtection;
    private Boolean         m_loopControlVariable;

    public VariableEntry() {
    }

    public VariableEntry(String name, TypeDefinition type, org.antlr.v4.runtime.ParserRuleContext ctx)
    {
        super(name);
        this.m_ctx = ctx;
        this.m_type = type;
        this.m_hasAssigmentProtection = false;
        this.m_loopControlVariable = false;
    }

    public VariableEntry(String name, TypeDefinition type, Boolean hasAssigmentProtection, SmallPearlParser.VariableDenotationContext ctx)
    {
        super(name);
        this.m_ctx = ctx;
        this.m_type = type;
        this.m_hasAssigmentProtection = hasAssigmentProtection;
        this.m_loopControlVariable = false;
    }

    public String toString(int level) {
        String assigmenProtection = this.m_hasAssigmentProtection ? "INV" : "";
        return indentString(level) + super.toString(level) + "var " + m_type + " " + assigmenProtection + (this.m_loopControlVariable ? " LC" :"");
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

    public Void setLoopControlVariable() {
        m_loopControlVariable = true;
        return null;
    }

    public Boolean getLoopControlVariable() { return m_loopControlVariable; }
}