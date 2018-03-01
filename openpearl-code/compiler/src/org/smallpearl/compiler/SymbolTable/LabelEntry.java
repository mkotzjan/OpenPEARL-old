package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.SmallPearlParser;

/****
 *
 * VariableEntry extends SymbolTableEntry by adding data fields to support
 * variables and parameters.  It has a boolean field indicating if this is a
 * reference-type symbol.  Reference-type symbols are definable in programming
 * languages with explicitly declared pointer types and/or call-by-reference
 * parameters.
 *									    <p>
 * VariableEntry also has an integer memory location field.  This can be either
 * an absolute address, or a relative offset, e.g., in a stack frame.
 *
 */
public class LabelEntry extends SymbolTableEntry {

    /**
     * Construct this with null data fields.
     */
    public LabelEntry() {
    }

    /**
     * Construct this with the given data field values.
     */
    public LabelEntry(String name, SmallPearlParser.Label_statementContext ctx) {
        super(name);
        this.m_ctx = ctx;
    }

    /**
     * Return the string rep of this, which consists of the return value of
     * super.toString, plus the values of this.isRef and this.memoryLocation.
     */
    public String toString(int level) {
        return indentString(level) + super.toString(level) + "label ";
    }
    public int getSourceLineNo() {
        return m_ctx.getStart().getLine();
    }

    public int getCharPositionInLine() {
        return m_ctx.getStart().getCharPositionInLine();
    }

    private SmallPearlParser.Label_statementContext m_ctx;
}