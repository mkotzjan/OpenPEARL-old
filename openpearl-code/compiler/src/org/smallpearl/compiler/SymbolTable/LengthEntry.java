package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.SmallPearlParser;
import org.smallpearl.compiler.TypeDefinition;
import org.smallpearl.compiler.TypeFixed;
import org.smallpearl.compiler.TypeFloat;
import org.smallpearl.compiler.TypeBit;
import org.smallpearl.compiler.TypeChar;

public class LengthEntry extends SymbolTableEntry {
    private TypeDefinition m_type;
    private SmallPearlParser.LengthDefinitionContext m_ctx;

    public LengthEntry() {
    }

    public LengthEntry(TypeDefinition type, SmallPearlParser.LengthDefinitionContext ctx) {
        this.m_ctx = ctx;
        this.m_type = type;
        this.setName("~LENGTH_" + type.getName() + "~");
    }

    public String toString(int level) {
        String s = indentString(level) + super.toString(level) + this.m_type.getName();
        if ( this.m_type instanceof TypeFixed) {
            s += "(" + ((TypeFixed)this.m_type).getPrecision() +")";
        }
        else if ( this.m_type instanceof TypeFloat) {
            s += "(" + ((TypeFloat)this.m_type).getPrecision() +")";
        }
        else if ( this.m_type instanceof TypeBit) {
            s += "(" + ((TypeBit)this.m_type).getPrecision() +")";
        }
        else if ( this.m_type instanceof TypeChar) {
            s += "(" + ((TypeChar)this.m_type).getSize() +")";
        }

        return s;
    }

    public int getSourceLineNo() {
        return m_ctx.getStart().getLine();
    }

    public int getCharPositionInLine() {
        return m_ctx.getStart().getCharPositionInLine();
    }

    public TypeDefinition getType() { return this.m_type; }
}
