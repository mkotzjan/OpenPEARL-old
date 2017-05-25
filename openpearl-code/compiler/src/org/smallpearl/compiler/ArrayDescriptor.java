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


import java.util.ArrayList;

public class ArrayDescriptor {
    private int m_noOfDimensions;
    private ArrayList<ArrayDimension> m_listOfDimensions;

    ArrayDescriptor() {
        this.m_noOfDimensions = 0;
        this.m_listOfDimensions = new ArrayList<ArrayDimension>();
    }

    ArrayDescriptor(int m_noOfDimensions, ArrayList<ArrayDimension> dimensions) {
        this.m_noOfDimensions = m_noOfDimensions;
        this.m_listOfDimensions = dimensions;
    }


    public String toString() {
        return "(" + this.getName() + ":" + Integer.toString(this.m_noOfDimensions) + this.m_listOfDimensions + ")";
    }


    public String getName() {
        String name = "ad";
        name += "_" + Integer.toString(this.m_noOfDimensions);

        for (int i = 0; i < m_listOfDimensions.size(); i++) {
            name += "_" + Integer.toString(m_listOfDimensions.get(i).getLowerBoundary());
            name += "_" + Integer.toString(m_listOfDimensions.get(i).getUpperBoundary());
        }

        return name;
    }

    public ArrayList<ArrayDimension> getDimensions() {
        return m_listOfDimensions;
    }

    public int getTotalNoOfElements() {
        int totalNoOfElements = 1;
        for (int i = 0; i < m_listOfDimensions.size(); i++) {
            totalNoOfElements *= m_listOfDimensions.get(i).getNoOfElements();
        }

        return totalNoOfElements;
    }

    @Override
    public boolean equals(Object other) {
        if (!(other instanceof ArrayDescriptor)) {
            return false;
        }

        ArrayDescriptor that = (ArrayDescriptor) other;

        // Custom equality check here.
        return this.m_noOfDimensions == that.m_noOfDimensions && this.m_listOfDimensions == that.m_listOfDimensions;
    }
}
