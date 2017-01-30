package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.SmallPearlParser;

public class BoltEntry extends SymbolTableEntry {
    public BoltEntry() {
    }

    public BoltEntry(String name, SmallPearlParser.BoltDeclarationContext ctx)
    {
        super(name);
        this.m_ctx = ctx;
    }

    public String toString(int level) {
        return indentString(level) + super.toString(level) + "var " + "BOLT";
    }
    public int getSourceLineNo() {
        return m_ctx.getStart().getLine();
    }
    public int getCharPositionInLine() {
        return m_ctx.getStart().getCharPositionInLine();
    }
    private SmallPearlParser.BoltDeclarationContext m_ctx;
}