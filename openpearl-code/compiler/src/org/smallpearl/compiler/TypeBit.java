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

public class TypeBit extends TypeDefinition {
    private int m_precision;

    TypeBit() {
        super("BIT");
        this.m_precision = Defaults.BIT_LENGTH;
    }

    TypeBit(int precision) {
        super("BIT");
        this.m_precision = precision;
    }

    public Integer getPrecision() {
        return m_precision;
    }

    public Void setPrecision(int precision) {
        m_precision = precision;
        return null;
    }

    public String toString() {
        return this.getName() + "(" + this.m_precision + ")";
    }

    public ST toST(STGroup group) {
        ST st = group.getInstanceOf("bit_type");
        st.add("size", m_precision);
        return st;
    }

    @Override
    public boolean equals(Object other) {
        if (!(other instanceof TypeBit)) {
            return false;
        }

        TypeBit that = (TypeBit) other;

        // Custom equality check here.
        return this.m_precision == that.m_precision;
    }
}
