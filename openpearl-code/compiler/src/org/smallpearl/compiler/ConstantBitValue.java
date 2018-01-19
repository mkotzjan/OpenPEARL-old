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

public class ConstantBitValue extends ConstantValue {
    private long m_value;
    private int m_no;
    private int m_noOfBits;


    ConstantBitValue(long value, int noOfBits) {
        m_noOfBits = noOfBits;
        m_value = value;
    }

/*
    ConstantBitValue(String str) {
        if( str.startsWith("'")) {
            str = str.substring(1, str.length());
        }

        if( str.endsWith("'")) {
            str = str.substring(0, str.length() - 1);
        }

        m_noOfBits = str.length();
        m_value =  str;
        m_no = -1;
    }
*/

    public void setNo(int no) { m_no = no; }

    public int getLength() { return m_noOfBits; }

    public long getLongValue() {
        return m_value;
    }

    public String getValue() {
        return "0x" + Long.toHexString(m_value);
    }

    public String getBaseType() {
        return "BitString";
    }

    public String toString() {
//        String name = "CONSTANT_" + getBaseType().toUpperCase();
//        name += "_" + m_value.length() + "_" + canonicalize(m_value);
        String name = "CONST_BITSTRING_" + m_no;
        return name;
    }

    public String canonicalize(String str) {
        String res = "";

        if( str.startsWith("'")) {
            str = str.substring(1, str.length());
        }

        if( str.endsWith("'")) {
            str = str.substring(0, str.length() - 1);
        }

        for ( int i = 0; i < str.length(); i++) {
            Character ch = str.charAt(i);

            if ( !(( ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z' ) || ( ch >= '0' && ch <= '9'))) {
                ch = '_';
            }

            res += String.valueOf(ch);

        }

        return res;
    }
}
