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

package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.SmallPearlParser;
import org.smallpearl.compiler.ExpressionResult;
import org.smallpearl.compiler.TypeDefinition;

import java.util.LinkedList;

public class ProcedureEntry extends SymbolTableEntry {

    private Boolean m_isMain;
    private String  m_globalId;
    private Integer m_priority;
    private LinkedList<FormalParameter> m_formalParameters;
    private ExpressionResult m_resultType;

    public ProcedureEntry() {
        this.m_formalParameters = null;
    }

    public ProcedureEntry(String name, LinkedList<FormalParameter> formalParameters, ExpressionResult resultType, String globalId, SmallPearlParser.ProcedureDeclarationContext ctx, SymbolTable scope) {
        super(name);
        this.m_formalParameters = formalParameters;
        this.m_ctx = ctx;
        this.scope = scope;
        this.m_globalId = globalId;
        this.m_resultType = resultType;
    }

    public String toString(int m_level) {
        Boolean prefixComma = false;
        String s = indentString(m_level) + super.toString(m_level) + "proc";

        if ( m_formalParameters != null && m_formalParameters.size() > 0 ) {
            s += "(";
            for (FormalParameter formalParameter : m_formalParameters) {
                if ( prefixComma ) {
                    s += ", ";
                } else {
                    s += " ";
                }

                s += formalParameter.toString();

                prefixComma = true;
            }
            s += " )";
        }

        if ( m_resultType != null ) {
            s += " returns " + m_resultType;
        }

        s += scopeString(m_level);

        return s;
    }

    protected String scopeString(int m_level) {
        return scope == null ? "\n" : "\n" + scope.toString(m_level);
    }
    public int getSourceLineNo() {
        return m_ctx.getStart().getLine();
    }
    public int getCharPositionInLine() {
        return m_ctx.getStart().getCharPositionInLine();
    }
    public TypeDefinition getResultType() { return m_resultType.getType(); }

    public SymbolTable scope;
    private  SmallPearlParser.ProcedureDeclarationContext m_ctx;
}