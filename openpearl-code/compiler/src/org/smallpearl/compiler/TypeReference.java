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


import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;

public class TypeReference extends TypeDefinition {
    private TypeDefinition m_baseType;

    TypeReference() {
        super("REF");
        this.m_baseType = null;
    }

    TypeReference(TypeDefinition type) {
        super("REF");
        this.m_baseType = type;
    }


    public Void setBaseType(TypeDefinition type) {
        this.m_baseType = type;
        return null;
    }

    public TypeDefinition getBaseType() {
        return this.m_baseType;
    }

    public String toString() {
        return this.getName() + " " + this.m_baseType;
    }

    @Override
    public boolean equals(Object other) {
        if (!(other instanceof TypeReference)) {
            return false;
        }

        TypeReference that = (TypeReference) other;

        // Custom equality check here.
        return this.m_baseType.equals(that.getBaseType());
    }

    public ST toST(STGroup group) {
        ST st = group.getInstanceOf("TypeReferenceSimpleType");
        st.add("BaseType", m_baseType.toST(group));
        return st;
    }

}
