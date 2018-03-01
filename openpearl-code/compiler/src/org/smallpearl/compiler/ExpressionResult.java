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

import org.smallpearl.compiler.SymbolTable.VariableEntry;

public class ExpressionResult {
    public TypeDefinition  m_type;
    public boolean         m_constant;
    public VariableEntry   m_variable;

    ExpressionResult(TypeDefinition type) {
        m_type = type;
        m_constant = false;
        m_variable = null;
    }

    ExpressionResult(TypeDefinition type, boolean constant) {
        m_type = type;
        m_constant = constant;
        m_variable = null;

    }

    ExpressionResult(TypeDefinition type, boolean constant, VariableEntry variable ) {
        m_type = type;
        m_constant = constant;
        m_variable = variable;
    }

    public boolean isConstant() { return this.m_constant; }
    public boolean isNotConstant() { return !this.isConstant(); }
    public TypeDefinition getType() { return this.m_type; }
    public VariableEntry getVariable() { return this.m_variable; }

    public String toString() {
        return "ExpressionResult: " + this.m_type + " " + this.isConstant() + " " + this.m_variable;
    }
}
