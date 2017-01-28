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

import org.stringtemplate.v4.ST;

public class Utils {


    public static
    Long convertBitStringToLong(String bitstring) {
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
    int getBitStringLength(String bitstring) {
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

        return bitstring.length() - 3;
    }

}
