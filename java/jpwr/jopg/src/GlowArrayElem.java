/* 
 * Proview   Open Source Process Control.
 * Copyright (C) 2005-2012 SSAB EMEA AB.
 *
 * This file is part of Proview.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with Proview. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking Proview statically or dynamically with other modules is
 * making a combined work based on Proview. Thus, the terms and 
 * conditions of the GNU General Public License cover the whole 
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * Proview give you permission to, from the build function in the
 * Proview Configurator, combine Proview with modules generated by the
 * Proview PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every 
 * copy of the combined work is accompanied by a complete copy of 
 * the source code of Proview (the version used to produce the 
 * combined work), being distributed under the terms of the GNU 
 * General Public License plus this exception.
 */


package jpwr.jopg;

public class GlowArrayElem {

    public int type() {return 0;}
    public void draw() {}
    public void draw(GlowTransform t, int highlight, int hot, Object node, Object colornode) {}
    public int eventHandler(GlowEvent e, double fx, double fy) {return 0;}
    public Object getUserData() { return null;}
    public void get_borders( GlowTransform t, GlowGeometry g) {}
    public int get_background_object_limits(GlowTransform t, int type, double x, double y, Object b) {return 0;}
    public int getClassDynType1() { return 0;}
    public int getClassDynType2() { return 0;}
    public int getClassActionType1() { return 0;}
    public int getClassActionType2() { return 0;}
    public int getClassCycle() { return 0;}
    public void setColorInverse(int color_inverse) {}
}