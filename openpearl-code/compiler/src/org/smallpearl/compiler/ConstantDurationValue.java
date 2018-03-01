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

public class ConstantDurationValue extends ConstantValue {
    private int m_hours = 0;
    private int m_minutes = 0;
    private double m_seconds = 0.0;

    ConstantDurationValue(int hours, int minutes, int seconds) {
        m_hours = hours;
        m_minutes = minutes;
        m_seconds = seconds;
    }

    ConstantDurationValue(int hours, int minutes, double seconds) {
        m_hours = hours;
        m_minutes = minutes;
        m_seconds = seconds;
    }

    public int getHours() { return m_hours;};
    public int getMinutes() { return m_minutes;};
    public double getSeconds() { return m_seconds;};

    public double getValue() {
        return m_hours * 3600 + m_minutes * 60 + m_seconds;
    }

    public String getBaseType() {
        return "Duration";
    }

    public String toString() {
        String name = "CONST_" + getBaseType().toUpperCase();
        double value = this.getValue();

        if ( value < 0 ) {
            name += "_N";
        }
        else if (value >= 0 ) {
            name += "_P";
        }

        name += "_" + m_hours + "_" + m_minutes + "_" + m_seconds;
        return name.replaceAll("\\.", "_");
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

        ConstantDurationValue other = (ConstantDurationValue) o;

        // field comparison
        return this.m_hours == other.m_hours &&
                this.m_minutes == other.m_minutes &&
                this.m_seconds == other.m_seconds;

    }
}


