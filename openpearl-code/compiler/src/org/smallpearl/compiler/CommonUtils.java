/*
 * [The "BSD license"]
 *  Copyright (c) 2016 Marcel Schaible
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

import java.util.LinkedList;
import java.util.UUID;

import org.smallpearl.compiler.SymbolTable.SymbolTable;
import org.smallpearl.compiler.SymbolTable.VariableEntry;
import org.stringtemplate.v4.ST;

public class CommonUtils {

    public static
    Long convertBitStringToLong(String bitstring) {
        int base = 0;
        int bitsPerPosition = 0;
        int noOfBits = 0;
        String postfix = "";

        if( bitstring.startsWith("'")) {
            bitstring = bitstring.substring(1, bitstring.length());
        }

        if( bitstring.endsWith("'")) {
            bitstring = bitstring.substring(0, bitstring.length() - 1);
        }

        postfix = bitstring.substring(bitstring.indexOf("'") + 1, bitstring.length() );
        bitstring = bitstring.substring(0, bitstring.indexOf("'"));

        if ( postfix.equals("B") || postfix.equals("B1")) {
            base =2;
            bitsPerPosition = 1;
        }
        else if ( postfix.equals("B2") ) {
            base = 4;
            bitsPerPosition = 2;
        }
        else if ( postfix.equals("B3") ) {
            base = 8;
            bitsPerPosition = 3;
        }
        else if ( postfix.equals("B4") ) {
            base = 16;
            bitsPerPosition = 4;
        }

        noOfBits = bitstring.length() * bitsPerPosition;

        Long i = 0L;
        for( int j = 0; j < bitstring.length(); j++) {
            int num = 0;
            switch( bitstring.charAt(j) ) {
                case '0': num =  0; break;
                case '1': num =  1; break;
                case '2': num =  2; break;
                case '3': num =  3; break;
                case '4': num =  4; break;
                case '5': num =  5; break;
                case '6': num =  6; break;
                case '7': num =  7; break;
                case '8': num =  8; break;
                case '9': num =  9; break;
                case 'A': num = 10; break;
                case 'B': num = 11; break;
                case 'C': num = 12; break;
                case 'D': num = 13; break;
                case 'E': num = 14; break;
                case 'F': num = 15; break;
            }

            i *= base;
            i += num;
        }

        return i;
    }

    public static String formatBitStringConstant(Long l, int numberOfBits) {
        String bitStringConstant;

        String b = Long.toBinaryString(l);
        String bres = "";

        int l1 = b.length();

        if (numberOfBits < b.length())

        {
            bres = "";
            for (int i = 0; i < numberOfBits; i++) {
                bres = bres + b.charAt(i);
            }
            Long r = Long.parseLong(bres, 2);
            if (Long.toBinaryString(Math.abs(r)).length() < 15) {
                bitStringConstant = "0x" + Long.toHexString(r).toString();
            } else if (Long.toBinaryString(Math.abs(r)).length() < 31) {
                bitStringConstant = "0x" + Long.toHexString(r).toString() + "UL";
            } else {
                bitStringConstant = "0x" + Long.toHexString(r).toString() + "ULL";
            }
        } else if (numberOfBits > b.length()) {
            bres = b;
            Long r = Long.parseLong(bres, 2);
            if (Long.toBinaryString(Math.abs(r)).length() < 15) {
                bitStringConstant = "0x" + Long.toHexString(r).toString();
            } else if (Long.toBinaryString(Math.abs(r)).length() < 31) {
                bitStringConstant = "0x" + Long.toHexString(r).toString() + "UL";
            } else {
                bitStringConstant = "0x" + Long.toHexString(r).toString() + "ULL";
            }
        } else
        {
            if (Long.toBinaryString(Math.abs(l)).length() < 15) {
                bitStringConstant = "0x" + Long.toHexString(l).toString();
            } else if (Long.toBinaryString(Math.abs(l)).length() < 31) {
                bitStringConstant = "0x" + Long.toHexString(l).toString() + "UL";
            } else {
                bitStringConstant = "0x" + Long.toHexString(l).toString() + "ULL";
            }
        }

        return bitStringConstant;
    }

    public static
    int getBitStringLength1(String bitstring) {
        int base = 0;
        StringBuilder sb = new StringBuilder(bitstring.length());

        if( bitstring.startsWith("'")) {
            bitstring = bitstring.substring(1, bitstring.length());
        }

        if( bitstring.endsWith("'")) {
            bitstring = bitstring.substring(0, bitstring.length() - 1);
        }

        if ( bitstring.charAt(bitstring.length() -1 ) == '1' ) {
            base = 2;
        }
        else if ( bitstring.charAt(bitstring.length() -1 ) == '2' ) {
            base = 4;
        }
        else if ( bitstring.charAt(bitstring.length() -1 ) == '3' ) {
            base = 8;
        }
        else if ( bitstring.charAt(bitstring.length() -1 ) == '4' ) {
            base = 16;
        }

        Long i = 0L;
        for( int j = 0; j < bitstring.length() - 3; j++) {
            int num = 0;
            switch( bitstring.charAt(j) ) {
                case '0': num =  0; break;
                case '1': num =  1; break;
                case '2': num =  2; break;
                case '3': num =  3; break;
                case '4': num =  4; break;
                case '5': num =  5; break;
                case '6': num =  6; break;
                case '7': num =  7; break;
                case '8': num =  8; break;
                case '9': num =  9; break;
                case 'A': num = 10; break;
                case 'B': num = 11; break;
                case 'C': num = 12; break;
                case 'D': num = 13; break;
                case 'E': num = 14; break;
                case 'F': num = 15; break;
            }

            i *= base;
            i += num;
        }

        return Long.toBinaryString(i).length();
    }

    public static
    int getBitStringLength(String bitstring) {
        int bitsPerPosition = 0;
        int noOfBits = 0;
        String postfix = "";

        if( bitstring.startsWith("'")) {
            bitstring = bitstring.substring(1, bitstring.length());
        }

        if( bitstring.endsWith("'")) {
            bitstring = bitstring.substring(0, bitstring.length() - 1);
        }

        postfix = bitstring.substring(bitstring.indexOf("'") + 1, bitstring.length() );
        bitstring = bitstring.substring(0, bitstring.indexOf("'"));

        if ( postfix.equals("B") || postfix.equals("B1")) {
            bitsPerPosition = 1;
        }
        else if ( postfix.equals("B2") ) {
            bitsPerPosition = 2;
        }
        else if ( postfix.equals("B3") ) {
            bitsPerPosition = 3;
        }
        else if ( postfix.equals("B4") ) {
            bitsPerPosition = 4;
        }

        noOfBits = bitstring.length() * bitsPerPosition;
        return  noOfBits;
    }

    public static
    String getUUIDString() {
        UUID uuid = UUID.randomUUID();
        String s = uuid.toString();
        s = s.replaceAll("-", "_");
        return s;
    }

    public static
    String getArrayDescriptorName(VariableEntry entry) {
        String name = "ad_";
        TypeArray type = ((TypeArray)entry.getType());

        return name;
    }

    public static
    String unescapeCppString(String st) {
        StringBuilder sb = new StringBuilder(st.length());

        for (int i = 0; i < st.length(); i++) {
            char ch = st.charAt(i);
            if (ch == '\\') {
                char nextChar = (i == st.length() - 1) ? '\\' : st.charAt(i + 1);

                switch (nextChar) {
                    case '"':
                        ch = '"';
                        i++;
                        break;
                }
            }
            sb.append(ch);
        }
        return sb.toString();
    }

    public static
    String unescapePearlString(String st) {
        StringBuilder sb = new StringBuilder(st.length());
        int state = 0;
        String value = "";

        for (int i = 0; i < st.length(); i++) {
            char ch = st.charAt(i);

            switch (state) {
                case 0:
                    if (ch == '\'') {
                        state = 1;
                    } else {
                        sb.append(ch);
                    }
                    break;

                case 1:
                    if (ch == '\\') {
                        state = 2;
                    }
                    else {
                        state = 0;
                        sb.append(ch);
                    }
                    break;

                case 2:
                    value += ch;
                    state = 3;
                    break;

                case 3:
                    value += ch;
                    state = 4;
                    break;

                case 4:
                    if (ch == '\\') {
                        state = 5;
                    }
                    else {
                        state = 0;
                        sb.append(ch);
                    }
                    break;

                case 5:
                    state = 0;
                    if (ch == '\'') {
                        String octalValue = "\\0" + Integer.toString(Integer.parseInt(value, 16), 8);
                        sb.append(octalValue);
                    } else {
                        sb.append(ch);
                    }
                    break;
            }
        }

        return sb.toString();
    }

    public static
    String remopveQuotes(String st) {
        st = st.replaceAll("^'", "");
        st = st.replaceAll("'$", "");
        return st;
    }

    public static
    int getStringLength(String st) {
        int state = 0;
        int len   = 0;

        for (int i = 0; i < st.length(); i++) {
            char ch = st.charAt(i);

            switch (state) {
                case 0:
                    if (ch == '\\') {
                        state = 1;
                    } else {
                        len++;
                    }
                    break;

                case 1:
                    if (ch == '0') {
                        state = 2;
                    }
                    else {
                        state = 0;
                        len++;
                    }
                    break;

                case 2:
                    if ( ch >= '0' && ch <= '7') {
                        state = 3;
                    }
                    else {
                        state = 0;
                        len++;
                    }
                    break;

                case 3:
                    if ( ch >= '0' && ch <= '7') {
                        state = 0;
                        len++;
                    }
                    else {
                        state = 0;
                        len++;
                    }
                    break;

            }
        }

        return len;
    }

}
