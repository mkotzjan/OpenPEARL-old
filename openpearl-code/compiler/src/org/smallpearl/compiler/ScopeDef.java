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

import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.NoSuchElementException;

public abstract class ScopeDef extends Definition {

    private int scopeNumber;

    private Hashtable elements;
    private int nrOfDef = 0;
    private LinkedList unknown = new LinkedList();

    public ScopeDef(String name, Position occ, ScopeDef parentScope) {
        super(name, occ, parentScope);
        elements = new Hashtable();
        scopeNumber = SymbolTable.getScopeCounter();
    }

    protected void addUnknown(Definition def) {
        unknown.add(def);
    }


    protected void add(Definition def) throws DoubleDeclarationException {
        Definition oldDef = (Definition)elements.get(def.getName());
        if (oldDef != null) {        // Wenn ja => Doppeldefinition
            if ( !(oldDef instanceof BlockDef))
                throw new DoubleDeclarationException(def, oldDef);
        }
        elements.put(def.getName(), def);
        def.setParentScope(this);

        if (def instanceof ConstantDef || def instanceof VariableDef) {
            def.setPosInScope(nrOfDef);
            nrOfDef++;
        }
    }


    public boolean hasElements() {
        return !elements.isEmpty();
    }

    public Hashtable getElements() {
        return elements;
    }

    public Iterator scopeIdentifiers() {
        return new Iterator() {
            private Iterator i = identifiers();
            private Definition sdCurrent, sdNext;

            public void remove() {
                throw new IllegalArgumentException();
            }

            public boolean hasNext() {
                return findNext(false);
            }

            public Object next() {
                if (!findNext(true)) {
                    throw new NoSuchElementException();
                }

                return sdCurrent;
            }

            private boolean findNext(boolean fGet) {
                if (sdNext != null) {
                    if (fGet) {
                        sdCurrent = sdNext;
                        sdNext = null;
                    }
                    return true;
                }

                while (i.hasNext()) {
                    Definition d = (Definition) i.next();
                    if (d instanceof ScopeDef) {
                        if (!fGet) {
                            sdNext = d;
                        } else {
                            sdCurrent = d;
                        }
                        return true;
                    }
                }
                return false;
            }

        };
    }

    public Iterator identifiers() {
        return new Iterator() {

            private Iterator i = elements.keySet().iterator();

            public void remove() {
                throw new IllegalArgumentException();
            }

            public boolean hasNext() {
                return i.hasNext();
            }

            public Object next() {
                return lookup((String) i.next());
            }

        };
    }

    Definition lookup(String name) {
        // Try to find the name in our scope
        Definition d = (Definition) elements.get(name);
        if (d == null && getParentScope() != null) {
            d = getParentScope().lookup(name);
        }
        return d;
    }

    public int getScopeNumber() {
        return scopeNumber;
    }

    public String toString() {
        return elements.toString();
    }

}
