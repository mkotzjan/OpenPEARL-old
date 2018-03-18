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

public class ConstantFloatValue extends ConstantValue {
    private Double   m_value;
    private Integer  m_precision;

    ConstantFloatValue(Float value, Integer precision) {
        m_value = value.doubleValue();
        m_precision = precision;
    }

    ConstantFloatValue(Double value, Integer precision) {
        m_value = value;
        m_precision = precision;
    }

    public double getValue() {
        return m_value;
    }

    public String getBaseType() {
        return "Float";
    }

    public int getPrecision() {
        return m_precision;
    }

    public void setPrecision(int precision) {
        m_precision = precision;
    }

    public String toString() {
        String name = "CONST_" + getBaseType().toUpperCase();
        Double value = Math.abs(m_value);

        if ( m_value < 0.0 ) {
            name += "_N";
        }
        else {
            name += "_P";
        }

        name += "_" + value.toString().replace('.','_').replace('-','_').replace('+','_') + "_" + m_precision.toString();

        return name;
    }

    @Override
    public boolean equals(Object o) {
        // self check
        if (this == o)
            return true;
        // null check
        if (o == null)
            return false;

        // type check and cast
        if (getClass() != o.getClass())
            return false;

        ConstantFloatValue other = (ConstantFloatValue) o;

        // field comparison
        return this.m_value == other.m_value &&
                this.m_precision == other.m_precision;

    }

}
