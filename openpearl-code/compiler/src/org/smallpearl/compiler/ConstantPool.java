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

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.stringtemplate.v4.ST;

import java.util.ArrayList;
import java.util.List;


public class ConstantPool {

    static List<ConstantValue> constantPool = new ArrayList<ConstantValue>();

    public ConstantPool() {
    }

    public Void add(ConstantValue value) {
        int i;
        int constantBitNo = 0;

        boolean found = false;

        for (i = 0; i < constantPool.size(); i++) {
            if ( value instanceof ConstantFixedValue) {
                if (constantPool.get(i) instanceof ConstantFixedValue) {
                    Long a = ((ConstantFixedValue)(value)).getValue();
                    Long b = ((ConstantFixedValue)(constantPool.get(i))).getValue();

                    if ( a.equals(b) ) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantFloatValue) {
                if (constantPool.get(i) instanceof ConstantFloatValue) {
                    if ( Double.compare( ((ConstantFloatValue)(value)).getValue(), ((ConstantFloatValue)(constantPool.get(i))).getValue()) == 0) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantCharacterValue) {
                if (constantPool.get(i) instanceof ConstantCharacterValue) {
                    String s1 = ((ConstantCharacterValue)(value)).getValue();
                    String s2 = ((ConstantCharacterValue)(constantPool.get(i))).getValue();

                    if ( s1.equals(s2)) {
                        found = true;
                        break;
                    }
                }
            }
            else if ( value instanceof ConstantBitValue) {
                if (constantPool.get(i) instanceof ConstantBitValue) {
                    constantBitNo = constantBitNo + 1;
                    String s1 = ((ConstantBitValue)(value)).getValue();
                    String s2 = ((ConstantBitValue)(constantPool.get(i))).getValue();

                    if ( s1.equals(s2)) {
                        found = true;
                        break;
                    }
                }
            }
        }

        if (!found) {
            if ( value instanceof ConstantBitValue) {
                constantBitNo = constantBitNo + 1;
                ((ConstantBitValue) (value)).setNo(constantBitNo);
            }

            constantPool.add(value);
        }

        return null;
    }

//    public ConstantValue lookup(String value) {
//        int i;
//        boolean found = false;
//
//        for (i = 0; i < constantPool.size(); i++) {
//            if ( value instanceof ConstantFixedValue) {
//                if (constantPool.get(i) instanceof ConstantFixedValue) {
//                    Long a = ((ConstantFixedValue)(value)).getValue();
//                    Long b = ((ConstantFixedValue)(constantPool.get(i))).getValue();
//
//                    if ( a.equals(b) ) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//            else if ( value instanceof ConstantFloatValue) {
//                if (constantPool.get(i) instanceof ConstantFloatValue) {
//                    if ( Double.compare( ((ConstantFloatValue)(value)).getValue(), ((ConstantFloatValue)(constantPool.get(i))).getValue()) == 0) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//            else if ( value instanceof ConstantCharacterValue) {
//                if (constantPool.get(i) instanceof ConstantCharacterValue) {
//                    String s1 = ((ConstantCharacterValue)(value)).getValue();
//                    String s2 = ((ConstantCharacterValue)(constantPool.get(i))).getValue();
//
//                    if ( s1.equals(s2)) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//            else if ( value instanceof ConstantBitValue) {
//                if (constantPool.get(i) instanceof ConstantBitValue) {
//                    constantBitNo = constantBitNo + 1;
//                    String s1 = ((ConstantBitValue)(value)).getValue();
//                    String s2 = ((ConstantBitValue)(constantPool.get(i))).getValue();
//
//                    if ( s1.equals(s2)) {
//                        found = true;
//                        break;
//                    }
//                }
//            }
//
//        }
//
//        if (!found) {
//            if ( value instanceof ConstantBitValue) {
//                constantBitNo = constantBitNo + 1;
//                ((ConstantBitValue) (value)).setNo(constantBitNo);
//            }
//
//            constantPool.add(value);
//        }
//
//        return null;
//    }


    public Void dump(){
        int i;
        System.out.println("");
        System.out.println("ConstantPool:");
        System.out.println("  Entries:" + constantPool.size());

        for ( i = 0; i < constantPool.size(); i++) {
            System.out.print( "    " + constantPool.get(i).toString());

            if ( constantPool.get(i) instanceof ConstantCharacterValue ) {
                ConstantCharacterValue str = (ConstantCharacterValue)constantPool.get(i);
                System.out.print( " : " + str.getValue());
            }

            System.out.println();
        }

        return null;
    }

    static public ConstantCharacterValue lookupCharacterValue(String value) {
        int i;

        for (i = 0; i < constantPool.size(); i++) {
            if (constantPool.get(i) instanceof ConstantCharacterValue) {
                String s = ((ConstantCharacterValue)(constantPool.get(i))).getValue();

                if ( s.equals(value)) {
                    return (ConstantCharacterValue)(constantPool.get(i));
                }
            }
        }

        return null;
    }

}
