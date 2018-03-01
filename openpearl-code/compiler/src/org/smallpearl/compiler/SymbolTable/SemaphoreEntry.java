package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.SmallPearlParser;

public class SemaphoreEntry extends SymbolTableEntry {
    public SemaphoreEntry() {
    }

    public SemaphoreEntry(String name, SmallPearlParser.SemaDeclarationContext ctx)
    {
        super(name);
        this.m_ctx = ctx;
    }

    public String toString(int level) {
        return indentString(level) + super.toString(level) + "var " + "SEMA";
    }
    public int getSourceLineNo() {
        return m_ctx.getStart().getLine();
    }
    public int getCharPositionInLine() {
        return m_ctx.getStart().getCharPositionInLine();
    }
    private SmallPearlParser.SemaDeclarationContext m_ctx;

}