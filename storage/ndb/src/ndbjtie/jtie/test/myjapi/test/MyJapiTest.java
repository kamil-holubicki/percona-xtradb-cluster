/*
 Copyright (c) 2010, 2011, Oracle and/or its affiliates. All rights reserved.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/
/*
 * MyJapiTest.java
 */

package test;

import java.io.PrintWriter;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.CharBuffer;
import java.nio.ShortBuffer;
import java.nio.IntBuffer;
import java.nio.LongBuffer;
import java.nio.FloatBuffer;
import java.nio.DoubleBuffer;
import java.nio.ReadOnlyBufferException;

import myjapi.MyJapi;
import myjapi.MyJapiCtypes;
import myjapi.A;
import myjapi.B0;
import myjapi.B1;
import myjapi.CI;
import myjapi.D0;
import myjapi.D1;
import myjapi.D2;
import myjapi.E;

/**
 * A comprehensive test of the mappings of primitive and user-defined types.
 *
 * In detail, this test consists of unit tests for
 * <ol>
 * <li> loading a dynamically linked system library
 * <li> calling most basic function: f0()
 * <li> platform-dependent mappings of all primitive types
 * <li> default mappings of primitive types
 * <li> ByteBuffer mappings of references of primitive types
 * <li> value-copy mappings of references of primitive types
 * <li> ByteBuffer<size=0> mappings of pointers to primitive types
 * <li> ByteBuffer<sizeof(C)> mappings of pointers to primitive types
 * <li> array<size=1> mappings of pointers to primitive types
 * <li> nullable ByteBuffer<size=0> mappings of pointers to primitive types
 * <li> nullable ByteBuffer<sizeof(C)> mappings of pointers to primitive types
 * <li> nullable array<size=0> mappings of pointers to primitive types
 * <li> nullable array<size=1> mappings of pointers to primitive types
 * <li> object mappings
 * <li> String mappings
 * <li> ByteBuffer<size=0> mappings of void/char pointers
 * <li> ByteBuffer<size=1> mappings of void/char pointers
 * <li> ByteBuffer<size=0> mappings of null-allowed void/char pointers
 * <li> n-ary functions
 * <li> const/inheritance mapping
 * </ol>
 *
 * Any errors are indicated by an exception; hence, the test succeeds when
 * finishing normally.
 */
public class MyJapiTest {

    static protected final PrintWriter out = new PrintWriter(System.out, true);

    static protected final PrintWriter err = new PrintWriter(System.err, true);

    // ensure that asserts are enabled
    static {
        boolean assertsEnabled = false;
        assert assertsEnabled = true; // intentional side effect
        if (!assertsEnabled) {
            throw new RuntimeException("Asserts must be enabled for this test to be effective!");
        }
    }

    /**
     * Loads a dynamically linked system library and reports any failures.
     */
    static protected void loadSystemLibrary(String name) {
        out.println("--> MyJapiTest.loadSystemLibrary(String)");

        final Class cls = MyJapiTest.class;
        out.println("    " + cls + " <" + cls.getClassLoader() + ">");

        out.println();
        out.println("    loading libary " + name + " ...");
        try {
            System.loadLibrary(name);
        } catch (UnsatisfiedLinkError e) {
            String path;
            try {
                path = System.getProperty("java.library.path");
            } catch (Exception ex) {
                path = "<exception caught: " + ex.getMessage() + ">";
            }
            err.println("failed loading library '"
                        + name + "'; java.library.path='" + path + "'");
            throw e;
        } catch (SecurityException e) {
            err.println("failed loading library '"
                        + name + "'; caught exception: " + e);
            throw e;
        }
        out.println("    ... loaded " + name);

        out.println();
        out.println("<-- MyJapiTest.loadSystemLibrary(String)");
    }

    static public void test0() {
        out.println("--> MyJapiTest.test0()");

        out.println("\ncalling most basic function: f0() ...");
        MyJapi.f0();

        out.println();
        out.println("<-- MyJapiTest.test0()");
    }

    static public void test1() {
        out.println("--> MyJapiTest.test1()");

        out.println("\ntesting platform-dependent mappings of all primitive types");
        out.println("\ncalling MyJapiCtypes functions: fxx(0) ...");

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f11()");
            final boolean nf11 = MyJapiCtypes.f11(false);
            assert (nf11 == false);
            out.println("\ncalling f12()");
            final byte nf12 = MyJapiCtypes.f12((byte)0);
            assert (nf12 == (byte)0);
            out.println("\ncalling f13()");
            final byte nf13 = MyJapiCtypes.f13((byte)0);
            assert (nf13 == (byte)0);
            out.println("\ncalling f14()");
            final byte nf14 = MyJapiCtypes.f14((byte)0);
            assert (nf14 == (byte)0);
            out.println("\ncalling f15()");
            final short nf15 = MyJapiCtypes.f15((short)0);
            assert (nf15 == (short)0);
            out.println("\ncalling f16()");
            final short nf16 = MyJapiCtypes.f16((short)0);
            assert (nf16 == (short)0);
            out.println("\ncalling f17()");
            final int nf17 = MyJapiCtypes.f17((int)0);
            assert (nf17 == (int)0);
            out.println("\ncalling f18()");
            final int nf18 = MyJapiCtypes.f18((int)0);
            assert (nf18 == (int)0);
            out.println("\ncalling f19()");
            final int nf19 = MyJapiCtypes.f19((int)0);
            assert (nf19 == (int)0);
            out.println("\ncalling f20()");
            final int nf20 = MyJapiCtypes.f20((int)0);
            assert (nf20 == (int)0);
            out.println("\ncalling f21()");
            final long nf21 = MyJapiCtypes.f21((long)0);
            assert (nf21 == (long)0);
            out.println("\ncalling f22()");
            final long nf22 = MyJapiCtypes.f22((long)0);
            assert (nf22 == (long)0);
            out.println("\ncalling f23()");
            final float nf23 = MyJapiCtypes.f23((float)0);
            assert (nf23 == (float)0);
            out.println("\ncalling f24()");
            final double nf24 = MyJapiCtypes.f24((double)0);
            assert (nf24 == (double)0);
            out.println("\ncalling f25()");
            final double nf25 = MyJapiCtypes.f25((double)0);
            assert (nf25 == (double)0);

            out.println("\ncalling f31()");
            final boolean nf31 = MyJapiCtypes.f31(false);
            assert (nf31 == false);
            out.println("\ncalling f32()");
            final byte nf32 = MyJapiCtypes.f32((byte)0);
            assert (nf32 == (byte)0);
            out.println("\ncalling f33()");
            final byte nf33 = MyJapiCtypes.f33((byte)0);
            assert (nf33 == (byte)0);
            out.println("\ncalling f34()");
            final byte nf34 = MyJapiCtypes.f34((byte)0);
            assert (nf34 == (byte)0);
            out.println("\ncalling f35()");
            final short nf35 = MyJapiCtypes.f35((short)0);
            assert (nf35 == (short)0);
            out.println("\ncalling f36()");
            final short nf36 = MyJapiCtypes.f36((short)0);
            assert (nf36 == (short)0);
            out.println("\ncalling f37()");
            final int nf37 = MyJapiCtypes.f37((int)0);
            assert (nf37 == (int)0);
            out.println("\ncalling f38()");
            final int nf38 = MyJapiCtypes.f38((int)0);
            assert (nf38 == (int)0);
            out.println("\ncalling f39()");
            final int nf39 = MyJapiCtypes.f39((int)0);
            assert (nf39 == (int)0);
            out.println("\ncalling f40()");
            final int nf40 = MyJapiCtypes.f40((int)0);
            assert (nf40 == (int)0);
            out.println("\ncalling f41()");
            final long nf41 = MyJapiCtypes.f41((long)0);
            assert (nf41 == (long)0);
            out.println("\ncalling f42()");
            final long nf42 = MyJapiCtypes.f42((long)0);
            assert (nf42 == (long)0);
            out.println("\ncalling f43()");
            final float nf43 = MyJapiCtypes.f43((float)0);
            assert (nf43 == (float)0);
            out.println("\ncalling f44()");
            final double nf44 = MyJapiCtypes.f44((double)0);
            assert (nf44 == (double)0);
            out.println("\ncalling f45()");
            final double nf45 = MyJapiCtypes.f45((double)0);
            assert (nf45 == (double)0);
        }

        out.println();
        out.println("<-- MyJapiTest.test1()");
    }

    static public void test2() {
        out.println("--> MyJapiTest.test2()");

        out.println("\ntesting default mappings of primitive types");
        out.println("\ncalling MyJapi functions: f1xx(f0xx()) ...");

        out.println("\ncalling f0()");
        MyJapi.f0();

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f111(f011())");
            MyJapi.f111(MyJapi.f011());
            out.println("\ncalling f112(f012())");
            MyJapi.f112(MyJapi.f012());
            out.println("\ncalling f113(f013())");
            MyJapi.f113(MyJapi.f013());
            out.println("\ncalling f114(f014())");
            MyJapi.f114(MyJapi.f014());
            out.println("\ncalling f115(f015())");
            MyJapi.f115(MyJapi.f015());
            out.println("\ncalling f116(f016())");
            MyJapi.f116(MyJapi.f016());
            out.println("\ncalling f117(f017())");
            MyJapi.f117(MyJapi.f017());
            out.println("\ncalling f118(f018())");
            MyJapi.f118(MyJapi.f018());
            out.println("\ncalling f121(f021())");
            MyJapi.f121(MyJapi.f021());
            out.println("\ncalling f122(f022())");
            MyJapi.f122(MyJapi.f022());
            out.println("\ncalling f123(f023())");
            MyJapi.f123(MyJapi.f023());
            out.println("\ncalling f124(f024())");
            MyJapi.f124(MyJapi.f024());

            out.println("\ncalling f131(f031())");
            MyJapi.f131(MyJapi.f031());
            out.println("\ncalling f132(f032())");
            MyJapi.f132(MyJapi.f032());
            out.println("\ncalling f133(f033())");
            MyJapi.f133(MyJapi.f033());
            out.println("\ncalling f134(f034())");
            MyJapi.f134(MyJapi.f034());
            out.println("\ncalling f135(f035())");
            MyJapi.f135(MyJapi.f035());
            out.println("\ncalling f136(f036())");
            MyJapi.f136(MyJapi.f036());
            out.println("\ncalling f137(f037())");
            MyJapi.f137(MyJapi.f037());
            out.println("\ncalling f138(f038())");
            MyJapi.f138(MyJapi.f038());
            out.println("\ncalling f141(f041())");
            MyJapi.f141(MyJapi.f041());
            out.println("\ncalling f142(f042())");
            MyJapi.f142(MyJapi.f042());
            out.println("\ncalling f143(f043())");
            MyJapi.f143(MyJapi.f043());
            out.println("\ncalling f144(f044())");
            MyJapi.f144(MyJapi.f044());
        }

        out.println();
        out.println("<-- MyJapiTest.test2()");
    }

    static public void test3bb() {
        out.println("--> MyJapiTest.test3bb()");

        out.println("\ntesting ByteBuffer mappings of references of primitive types");
        out.println("\ncalling MyJapi functions: f3xxbb(f2xxbb()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException
        //   java/nio/ReadOnlyBufferException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f237bb()");
            ByteBuffer bb = MyJapi.f237bb();
            out.println("bb = " + bb);
            bb.order(ByteOrder.nativeOrder()); // initial order is big endian!
            IntBuffer ib = bb.asIntBuffer();
            out.println("ib = " + ib);
            out.println("ib.get() = " + ib.get());
            out.println("\ncalling f337bb()");
            MyJapi.f337bb(bb);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f311bb(f211bb())");
            final ByteBuffer f211bb = MyJapi.f211bb().order(ByteOrder.nativeOrder());
            final byte nf211bb = f211bb.asReadOnlyBuffer().get();
            MyJapi.f311bb(f211bb);
            assert (nf211bb == f211bb.asReadOnlyBuffer().get());
            MyJapi.f311bb(MyJapi.f211bb());
            assert (nf211bb != f211bb.asReadOnlyBuffer().get());

            out.println("\ncalling f312bb(f212bb())");
            final ByteBuffer f212bb = MyJapi.f212bb().order(ByteOrder.nativeOrder());
            final byte nf212bb = f212bb.asReadOnlyBuffer().get();
            MyJapi.f312bb(f212bb);
            assert (nf212bb == f212bb.asReadOnlyBuffer().get());
            MyJapi.f312bb(MyJapi.f212bb());
            assert (nf212bb + 1 == f212bb.asReadOnlyBuffer().get());

            out.println("\ncalling f313bb(f213bb())");
            final ByteBuffer f213bb = MyJapi.f213bb().order(ByteOrder.nativeOrder());
            final byte nf213bb = f213bb.asReadOnlyBuffer().get();
            MyJapi.f313bb(f213bb);
            assert (nf213bb == f213bb.asReadOnlyBuffer().get());
            MyJapi.f313bb(MyJapi.f213bb());
            assert (nf213bb + 1 == f213bb.asReadOnlyBuffer().get());

            out.println("\ncalling f314bb(f214bb())");
            final ByteBuffer f214bb = MyJapi.f214bb().order(ByteOrder.nativeOrder());
            final byte nf214bb = f214bb.asReadOnlyBuffer().get();
            MyJapi.f314bb(f214bb);
            assert (nf214bb == f214bb.asReadOnlyBuffer().get());
            MyJapi.f314bb(MyJapi.f214bb());
            assert (nf214bb + 1 == f214bb.asReadOnlyBuffer().get());

            out.println("\ncalling f315bb(f215bb())");
            final ByteBuffer f215bb = MyJapi.f215bb().order(ByteOrder.nativeOrder());
            final short nf215bb = f215bb.asShortBuffer().get();
            MyJapi.f315bb(f215bb);
            assert (nf215bb == f215bb.asShortBuffer().get());
            MyJapi.f315bb(MyJapi.f215bb());
            assert (nf215bb + 1 == f215bb.asShortBuffer().get());

            out.println("\ncalling f316bb(f216bb())");
            final ByteBuffer f216bb = MyJapi.f216bb().order(ByteOrder.nativeOrder());
            final short nf216bb = f216bb.asShortBuffer().get();
            MyJapi.f316bb(f216bb);
            assert (nf216bb == f216bb.asShortBuffer().get());
            MyJapi.f316bb(MyJapi.f216bb());
            assert (nf216bb + 1 == f216bb.asShortBuffer().get());

            out.println("\ncalling f317bb(f217bb())");
            final ByteBuffer f217bb = MyJapi.f217bb().order(ByteOrder.nativeOrder());
            final int nf217bb = f217bb.asIntBuffer().get();
            MyJapi.f317bb(f217bb);
            assert (nf217bb == f217bb.asIntBuffer().get());
            MyJapi.f317bb(MyJapi.f217bb());
            assert (nf217bb + 1 == f217bb.asIntBuffer().get());

            out.println("\ncalling f318bb(f218bb())");
            final ByteBuffer f218bb = MyJapi.f218bb().order(ByteOrder.nativeOrder());
            final int nf218bb = f218bb.asIntBuffer().get();
            MyJapi.f318bb(f218bb);
            assert (nf218bb == f218bb.asIntBuffer().get());
            MyJapi.f318bb(MyJapi.f218bb());
            assert (nf218bb + 1 == f218bb.asIntBuffer().get());

            out.println("\ncalling f321bb(f221bb())");
            final ByteBuffer f221bb = MyJapi.f221bb().order(ByteOrder.nativeOrder());
            final long nf221bb = f221bb.asLongBuffer().get();
            MyJapi.f321bb(f221bb);
            assert (nf221bb == f221bb.asLongBuffer().get());
            MyJapi.f321bb(MyJapi.f221bb());
            assert (nf221bb + 1 == f221bb.asLongBuffer().get());

            out.println("\ncalling f322bb(f222bb())");
            final ByteBuffer f222bb = MyJapi.f222bb().order(ByteOrder.nativeOrder());
            final long nf222bb = f222bb.asLongBuffer().get();
            MyJapi.f322bb(f222bb);
            assert (nf222bb == f222bb.asLongBuffer().get());
            MyJapi.f322bb(MyJapi.f222bb());
            assert (nf222bb + 1 == f222bb.asLongBuffer().get());

            out.println("\ncalling f323bb(f223bb())");
            final ByteBuffer f223bb = MyJapi.f223bb().order(ByteOrder.nativeOrder());
            final float nf223bb = f223bb.asFloatBuffer().get();
            MyJapi.f323bb(f223bb);
            assert (nf223bb == f223bb.asFloatBuffer().get());
            MyJapi.f323bb(MyJapi.f223bb());
            assert (nf223bb + 1 == f223bb.asFloatBuffer().get());

            out.println("\ncalling f324bb(f224bb())");
            final ByteBuffer f224bb = MyJapi.f224bb().order(ByteOrder.nativeOrder());
            final double nf224bb = f224bb.asDoubleBuffer().get();
            MyJapi.f324bb(f224bb);
            assert (nf224bb == f224bb.asDoubleBuffer().get());
            MyJapi.f324bb(MyJapi.f224bb());
            assert (nf224bb + 1 == f224bb.asDoubleBuffer().get());

            out.println("\ncalling f331bb(f231bb())");
            final ByteBuffer f231bb = MyJapi.f231bb().order(ByteOrder.nativeOrder());
            final byte nf231bb = f231bb.asReadOnlyBuffer().get();
            MyJapi.f331bb(f231bb);
            assert (nf231bb != f231bb.asReadOnlyBuffer().get());

            out.println("\ncalling f332bb(f232bb())");
            final ByteBuffer f232bb = MyJapi.f232bb().order(ByteOrder.nativeOrder());
            final byte nf232bb = f232bb.asReadOnlyBuffer().get();
            MyJapi.f332bb(f232bb);
            assert (nf232bb + 1 == f232bb.asReadOnlyBuffer().get());

            out.println("\ncalling f333bb(f233bb())");
            final ByteBuffer f233bb = MyJapi.f233bb().order(ByteOrder.nativeOrder());
            final byte nf233bb = f233bb.asReadOnlyBuffer().get();
            MyJapi.f333bb(f233bb);
            assert (nf233bb + 1 == f233bb.asReadOnlyBuffer().get());

            out.println("\ncalling f334bb(f234bb())");
            final ByteBuffer f234bb = MyJapi.f234bb().order(ByteOrder.nativeOrder());
            final byte nf234bb = f234bb.asReadOnlyBuffer().get();
            MyJapi.f334bb(f234bb);
            assert (nf234bb + 1 == f234bb.asReadOnlyBuffer().get());

            out.println("\ncalling f335bb(f235bb())");
            final ByteBuffer f235bb = MyJapi.f235bb().order(ByteOrder.nativeOrder());
            final short nf235bb = f235bb.asShortBuffer().get();
            MyJapi.f335bb(f235bb);
            assert (nf235bb + 1 == f235bb.asShortBuffer().get());

            out.println("\ncalling f336bb(f236bb())");
            final ByteBuffer f236bb = MyJapi.f236bb().order(ByteOrder.nativeOrder());
            final short nf236bb = f236bb.asShortBuffer().get();
            MyJapi.f336bb(f236bb);
            assert (nf236bb + 1 == f236bb.asShortBuffer().get());

            out.println("\ncalling f337bb(f237bb())");
            final ByteBuffer f237bb = MyJapi.f237bb().order(ByteOrder.nativeOrder());
            final int nf237bb = f237bb.asIntBuffer().get();
            MyJapi.f337bb(f237bb);
            assert (nf237bb + 1 == f237bb.asIntBuffer().get());

            out.println("\ncalling f338bb(f238bb())");
            final ByteBuffer f238bb = MyJapi.f238bb().order(ByteOrder.nativeOrder());
            final int nf238bb = f238bb.asIntBuffer().get();
            MyJapi.f338bb(f238bb);
            assert (nf238bb + 1 == f238bb.asIntBuffer().get());

            out.println("\ncalling f341bb(f241bb())");
            final ByteBuffer f241bb = MyJapi.f241bb().order(ByteOrder.nativeOrder());
            final long nf241bb = f241bb.asLongBuffer().get();
            MyJapi.f341bb(f241bb);
            assert (nf241bb + 1 == f241bb.asLongBuffer().get());

            out.println("\ncalling f342bb(f242bb())");
            final ByteBuffer f242bb = MyJapi.f242bb().order(ByteOrder.nativeOrder());
            final long nf242bb = f242bb.asLongBuffer().get();
            MyJapi.f342bb(f242bb);
            assert (nf242bb + 1 == f242bb.asLongBuffer().get());

            out.println("\ncalling f343bb(f243bb())");
            final ByteBuffer f243bb = MyJapi.f243bb().order(ByteOrder.nativeOrder());
            final float nf243bb = f243bb.asFloatBuffer().get();
            MyJapi.f343bb(f243bb);
            assert (nf243bb + 1 == f243bb.asFloatBuffer().get());

            out.println("\ncalling f344bb(f244bb())");
            final ByteBuffer f244bb = MyJapi.f244bb().order(ByteOrder.nativeOrder());
            final double nf244bb = f244bb.asDoubleBuffer().get();
            MyJapi.f344bb(f244bb);
            assert (nf244bb + 1 == f244bb.asDoubleBuffer().get());
        }

        out.println();
        out.println("<-- MyJapiTest.test3bb()");
    }

    static public void test3v() {
        out.println("--> MyJapiTest.test3v()");

        out.println("\ntesting value-copy mappings of references of primitive types");
        out.println("\ncalling MyJapi functions: f3xxv(f2xxv()) ...");

        // XXX to do: check NULL argument, array length != 1 arg

        if (false) {
            // for testing/debugging
            out.println("\ncalling f337bb(f237bb()); f237v()");
            final int nf237v0 = MyJapi.f237v();
            final int[] nf337v = { nf237v0 };
            MyJapi.f337v(nf337v);
            assert (nf237v0 + 1 == nf337v[0]);
            final int nf237v1 = MyJapi.f237v();
            assert (nf237v1 == nf337v[0]);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f311v(f211v()); f311v(f211v())");
            final boolean nf211v0 = MyJapi.f211v();
            MyJapi.f311v(nf211v0);
            final boolean nf211v1 = MyJapi.f211v();
            MyJapi.f311v(nf211v1);
            assert (!nf211v0 == nf211v1);

            out.println("\ncalling f312v(f212v()); f312v(f212v())");
            final byte nf212v0 = MyJapi.f212v();
            MyJapi.f312v(nf212v0);
            final byte nf212v1 = MyJapi.f212v();
            MyJapi.f312v(nf212v1);
            assert (nf212v0 + 1 == nf212v1);

            out.println("\ncalling f313v(f213v()); f313v(f213v())");
            final byte nf213v0 = MyJapi.f213v();
            MyJapi.f313v(nf213v0);
            final byte nf213v1 = MyJapi.f213v();
            MyJapi.f313v(nf213v1);
            assert (nf213v0 + 1 == nf213v1);

            out.println("\ncalling f314v(f214v()); f314v(f214v())");
            final byte nf214v0 = MyJapi.f214v();
            MyJapi.f314v(nf214v0);
            final byte nf214v1 = MyJapi.f214v();
            MyJapi.f314v(nf214v1);
            assert (nf214v0 + 1 == nf214v1);

            out.println("\ncalling f315v(f215v()); f315v(f215v())");
            final short nf215v0 = MyJapi.f215v();
            MyJapi.f315v(nf215v0);
            final short nf215v1 = MyJapi.f215v();
            MyJapi.f315v(nf215v1);
            assert (nf215v0 + 1 == nf215v1);

            out.println("\ncalling f316v(f216v()); f316v(f216v())");
            final short nf216v0 = MyJapi.f216v();
            MyJapi.f316v(nf216v0);
            final short nf216v1 = MyJapi.f216v();
            MyJapi.f316v(nf216v1);
            assert (nf216v0 + 1 == nf216v1);

            out.println("\ncalling f317v(f217v()); f317v(f217v())");
            final int nf217v0 = MyJapi.f217v();
            MyJapi.f317v(nf217v0);
            final int nf217v1 = MyJapi.f217v();
            MyJapi.f317v(nf217v1);
            assert (nf217v0 + 1 == nf217v1);

            out.println("\ncalling f318v(f218v()); f318v(f218v())");
            final int nf218v0 = MyJapi.f218v();
            MyJapi.f318v(nf218v0);
            final int nf218v1 = MyJapi.f218v();
            MyJapi.f318v(nf218v1);
            assert (nf218v0 + 1 == nf218v1);

            out.println("\ncalling f321v(f221v()); f321v(f221v())");
            final long nf221v0 = MyJapi.f221v();
            MyJapi.f321v(nf221v0);
            final long nf221v1 = MyJapi.f221v();
            MyJapi.f321v(nf221v1);
            assert (nf221v0 + 1 == nf221v1);

            out.println("\ncalling f322v(f222v()); f322v(f222v())");
            final long nf222v0 = MyJapi.f222v();
            MyJapi.f322v(nf222v0);
            final long nf222v1 = MyJapi.f222v();
            MyJapi.f322v(nf222v1);
            assert (nf222v0 + 1 == nf222v1);

            out.println("\ncalling f323v(f223v()); f323v(f223v())");
            final float nf223v0 = MyJapi.f223v();
            MyJapi.f323v(nf223v0);
            final float nf223v1 = MyJapi.f223v();
            MyJapi.f323v(nf223v1);
            assert (nf223v0 + 1 == nf223v1);

            out.println("\ncalling f324v(f224v()); f324v(f224v())");
            final double nf224v0 = MyJapi.f224v();
            MyJapi.f324v(nf224v0);
            final double nf224v1 = MyJapi.f224v();
            MyJapi.f324v(nf224v1);
            assert (nf224v0 + 1 == nf224v1);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f331v(f231v()); f231v()");
            final boolean nf231v0 = MyJapi.f231v();
            final boolean[] nf331v = { nf231v0 };
            MyJapi.f331v(nf331v);
            assert (!nf231v0 == nf331v[0]);
            final boolean nf231v1 = MyJapi.f231v();
            assert (nf231v1 == nf331v[0]);

            out.println("\ncalling f332v(f232v()); f232v()");
            final byte nf232v0 = MyJapi.f232v();
            final byte[] nf332v = { nf232v0 };
            MyJapi.f332v(nf332v);
            assert (nf232v0 + 1 == nf332v[0]);
            final byte nf232v1 = MyJapi.f232v();
            assert (nf232v1 == nf332v[0]);

            out.println("\ncalling f333v(f233v()); f233v()");
            final byte nf233v0 = MyJapi.f233v();
            final byte[] nf333v = { nf233v0 };
            MyJapi.f333v(nf333v);
            assert (nf233v0 + 1 == nf333v[0]);
            final byte nf233v1 = MyJapi.f233v();
            assert (nf233v1 == nf333v[0]);

            out.println("\ncalling f334v(f234v()); f234v()");
            final byte nf234v0 = MyJapi.f234v();
            final byte[] nf334v = { nf234v0 };
            MyJapi.f334v(nf334v);
            assert (nf234v0 + 1 == nf334v[0]);
            final byte nf234v1 = MyJapi.f234v();
            assert (nf234v1 == nf334v[0]);

            out.println("\ncalling f335v(f235v()); f235v()");
            final short nf235v0 = MyJapi.f235v();
            final short[] nf335v = { nf235v0 };
            MyJapi.f335v(nf335v);
            assert (nf235v0 + 1 == nf335v[0]);
            final short nf235v1 = MyJapi.f235v();
            assert (nf235v1 == nf335v[0]);

            out.println("\ncalling f336v(f236v()); f236v()");
            final short nf236v0 = MyJapi.f236v();
            final short[] nf336v = { nf236v0 };
            MyJapi.f336v(nf336v);
            assert (nf236v0 + 1 == nf336v[0]);
            final short nf236v1 = MyJapi.f236v();
            assert (nf236v1 == nf336v[0]);

            out.println("\ncalling f337v(f237v()); f237v()");
            final int nf237v0 = MyJapi.f237v();
            final int[] nf337v = { nf237v0 };
            MyJapi.f337v(nf337v);
            assert (nf237v0 + 1 == nf337v[0]);
            final int nf237v1 = MyJapi.f237v();
            assert (nf237v1 == nf337v[0]);

            out.println("\ncalling f338v(f238v()); f238v()");
            final int nf238v0 = MyJapi.f238v();
            final int[] nf338v = { nf238v0 };
            MyJapi.f338v(nf338v);
            assert (nf238v0 + 1 == nf338v[0]);
            final int nf238v1 = MyJapi.f238v();
            assert (nf238v1 == nf338v[0]);

            out.println("\ncalling f341v(f241v()); f241v()");
            final long nf241v0 = MyJapi.f241v();
            final long[] nf341v = { nf241v0 };
            MyJapi.f341v(nf341v);
            assert (nf241v0 + 1 == nf341v[0]);
            final long nf241v1 = MyJapi.f241v();
            assert (nf241v1 == nf341v[0]);

            out.println("\ncalling f342v(f242v()); f242v()");
            final long nf242v0 = MyJapi.f242v();
            final long[] nf342v = { nf242v0 };
            MyJapi.f342v(nf342v);
            assert (nf242v0 + 1 == nf342v[0]);
            final long nf242v1 = MyJapi.f242v();
            assert (nf242v1 == nf342v[0]);

            out.println("\ncalling f343v(f243v()); f243v()");
            final float nf243v0 = MyJapi.f243v();
            final float[] nf343v = { nf243v0 };
            MyJapi.f343v(nf343v);
            assert (nf243v0 + 1 == nf343v[0]);
            final float nf243v1 = MyJapi.f243v();
            assert (nf243v1 == nf343v[0]);

            out.println("\ncalling f344v(f244v()); f244v()");
            final double nf244v0 = MyJapi.f244v();
            final double[] nf344v = { nf244v0 };
            MyJapi.f344v(nf344v);
            assert (nf244v0 + 1 == nf344v[0]);
            final double nf244v1 = MyJapi.f244v();
            assert (nf244v1 == nf344v[0]);
        }

        out.println();
        out.println("<-- MyJapiTest.test3v()");
    }

    static public void test4bb0() {
        out.println("--> MyJapiTest.test4bb0()");

        out.println("\ntesting ByteBuffer<size=0> mappings of pointers to primitive types");
        out.println("\ncalling MyJapi functions: f5xxbb0(f4xxbb0()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException
        //   java/nio/ReadOnlyBufferException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f437bb0()");
            ByteBuffer bb = MyJapi.f437bb0();
            out.println("bb = " + bb);
            out.println("\ncalling f537bb0()");
            MyJapi.f537bb0(bb);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f511bb0(f411bb0())");
            MyJapi.f511bb0(MyJapi.f411bb0());

            out.println("\ncalling f512bb0(f412bb0())");
            MyJapi.f512bb0(MyJapi.f412bb0());

            out.println("\ncalling f513bb0(f413bb0())");
            MyJapi.f513bb0(MyJapi.f413bb0());

            out.println("\ncalling f514bb0(f414bb0())");
            MyJapi.f514bb0(MyJapi.f414bb0());

            out.println("\ncalling f515bb0(f415bb0())");
            MyJapi.f515bb0(MyJapi.f415bb0());

            out.println("\ncalling f516bb0(f416bb0())");
            MyJapi.f516bb0(MyJapi.f416bb0());

            out.println("\ncalling f517bb0(f417bb0())");
            MyJapi.f517bb0(MyJapi.f417bb0());

            out.println("\ncalling f518bb0(f418bb0())");
            MyJapi.f518bb0(MyJapi.f418bb0());

            out.println("\ncalling f521bb0(f421bb0())");
            MyJapi.f521bb0(MyJapi.f421bb0());

            out.println("\ncalling f522bb0(f422bb0())");
            MyJapi.f522bb0(MyJapi.f422bb0());

            out.println("\ncalling f523bb0(f423bb0())");
            MyJapi.f523bb0(MyJapi.f423bb0());

            out.println("\ncalling f524bb0(f424bb0())");
            MyJapi.f524bb0(MyJapi.f424bb0());

            out.println("\ncalling f531bb0(f431bb0())");
            MyJapi.f531bb0(MyJapi.f431bb0());

            out.println("\ncalling f532bb0(f432bb0())");
            MyJapi.f532bb0(MyJapi.f432bb0());

            out.println("\ncalling f533bb0(f433bb0())");
            MyJapi.f533bb0(MyJapi.f433bb0());

            out.println("\ncalling f534bb0(f434bb0())");
            MyJapi.f534bb0(MyJapi.f434bb0());

            out.println("\ncalling f535bb0(f435bb0())");
            MyJapi.f535bb0(MyJapi.f435bb0());

            out.println("\ncalling f536bb0(f436bb0())");
            MyJapi.f536bb0(MyJapi.f436bb0());

            out.println("\ncalling f537bb0(f437bb0())");
            MyJapi.f537bb0(MyJapi.f437bb0());

            out.println("\ncalling f538bb0(f438bb0())");
            MyJapi.f538bb0(MyJapi.f438bb0());

            out.println("\ncalling f541bb0(f441bb0())");
            MyJapi.f541bb0(MyJapi.f441bb0());

            out.println("\ncalling f542bb0(f442bb0())");
            MyJapi.f542bb0(MyJapi.f442bb0());

            out.println("\ncalling f543bb0(f443bb0())");
            MyJapi.f543bb0(MyJapi.f443bb0());

            out.println("\ncalling f544bb0(f444bb0())");
            MyJapi.f544bb0(MyJapi.f444bb0());

            out.println("\ncalling f551bb0(f451bb0())");
            MyJapi.f551bb0(MyJapi.f451bb0());

            out.println("\ncalling f552bb0(f452bb0())");
            MyJapi.f552bb0(MyJapi.f452bb0());

            out.println("\ncalling f553bb0(f453bb0())");
            MyJapi.f553bb0(MyJapi.f453bb0());

            out.println("\ncalling f554bb0(f454bb0())");
            MyJapi.f554bb0(MyJapi.f454bb0());

            out.println("\ncalling f555bb0(f455bb0())");
            MyJapi.f555bb0(MyJapi.f455bb0());

            out.println("\ncalling f556bb0(f456bb0())");
            MyJapi.f556bb0(MyJapi.f456bb0());

            out.println("\ncalling f557bb0(f457bb0())");
            MyJapi.f557bb0(MyJapi.f457bb0());

            out.println("\ncalling f558bb0(f458bb0())");
            MyJapi.f558bb0(MyJapi.f458bb0());

            out.println("\ncalling f561bb0(f461bb0())");
            MyJapi.f561bb0(MyJapi.f461bb0());

            out.println("\ncalling f562bb0(f462bb0())");
            MyJapi.f562bb0(MyJapi.f462bb0());

            out.println("\ncalling f563bb0(f463bb0())");
            MyJapi.f563bb0(MyJapi.f463bb0());

            out.println("\ncalling f564bb0(f464bb0())");
            MyJapi.f564bb0(MyJapi.f464bb0());

            out.println("\ncalling f571bb0(f471bb0())");
            MyJapi.f571bb0(MyJapi.f471bb0());

            out.println("\ncalling f572bb0(f472bb0())");
            MyJapi.f572bb0(MyJapi.f472bb0());

            out.println("\ncalling f573bb0(f473bb0())");
            MyJapi.f573bb0(MyJapi.f473bb0());

            out.println("\ncalling f574bb0(f474bb0())");
            MyJapi.f574bb0(MyJapi.f474bb0());

            out.println("\ncalling f575bb0(f475bb0())");
            MyJapi.f575bb0(MyJapi.f475bb0());

            out.println("\ncalling f576bb0(f476bb0())");
            MyJapi.f576bb0(MyJapi.f476bb0());

            out.println("\ncalling f577bb0(f477bb0())");
            MyJapi.f577bb0(MyJapi.f477bb0());

            out.println("\ncalling f578bb0(f478bb0())");
            MyJapi.f578bb0(MyJapi.f478bb0());

            out.println("\ncalling f581bb0(f481bb0())");
            MyJapi.f581bb0(MyJapi.f481bb0());

            out.println("\ncalling f582bb0(f482bb0())");
            MyJapi.f582bb0(MyJapi.f482bb0());

            out.println("\ncalling f583bb0(f483bb0())");
            MyJapi.f583bb0(MyJapi.f483bb0());

            out.println("\ncalling f584bb0(f484bb0())");
            MyJapi.f584bb0(MyJapi.f484bb0());
        }

        out.println();
        out.println("<-- MyJapiTest.test4bb0()");
    }

    static public void test4bb1() {
        out.println("--> MyJapiTest.test4bb1()");

        out.println("\ntesting ByteBuffer<sizeof(C)> mappings of pointers to primitive types");
        out.println("\ncalling MyJapi functions: f5xxbb1(f4xxbb1()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException
        //   java/nio/ReadOnlyBufferException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f437bb1()");
            ByteBuffer bb = MyJapi.f437bb1();
            out.println("bb = " + bb);
            bb.order(ByteOrder.nativeOrder()); // initial order is big endian!
            IntBuffer ib = bb.asIntBuffer();
            out.println("ib = " + ib);
            out.println("ib.get() = " + ib.get());
            out.println("\ncalling f537bb1()");
            MyJapi.f537bb1(bb);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f511bb1(f411bb1())");
            final ByteBuffer f411bb = MyJapi.f411bb1().order(ByteOrder.nativeOrder());
            final byte nf411bb = f411bb.asReadOnlyBuffer().get();
            MyJapi.f511bb1(f411bb);
            assert (nf411bb == f411bb.asReadOnlyBuffer().get());
            MyJapi.f511bb1(MyJapi.f411bb1());
            assert (nf411bb != f411bb.asReadOnlyBuffer().get());

            out.println("\ncalling f512bb1(f412bb1())");
            final ByteBuffer f412bb = MyJapi.f412bb1().order(ByteOrder.nativeOrder());
            final byte nf412bb = f412bb.asReadOnlyBuffer().get();
            MyJapi.f512bb1(f412bb);
            assert (nf412bb == f412bb.asReadOnlyBuffer().get());
            MyJapi.f512bb1(MyJapi.f412bb1());
            assert (nf412bb + 1 == f412bb.asReadOnlyBuffer().get());

            out.println("\ncalling f513bb1(f413bb1())");
            final ByteBuffer f413bb = MyJapi.f413bb1().order(ByteOrder.nativeOrder());
            final byte nf413bb = f413bb.asReadOnlyBuffer().get();
            MyJapi.f513bb1(f413bb);
            assert (nf413bb == f413bb.asReadOnlyBuffer().get());
            MyJapi.f513bb1(MyJapi.f413bb1());
            assert (nf413bb + 1 == f413bb.asReadOnlyBuffer().get());

            out.println("\ncalling f514bb1(f414bb1())");
            final ByteBuffer f414bb = MyJapi.f414bb1().order(ByteOrder.nativeOrder());
            final byte nf414bb = f414bb.asReadOnlyBuffer().get();
            MyJapi.f514bb1(f414bb);
            assert (nf414bb == f414bb.asReadOnlyBuffer().get());
            MyJapi.f514bb1(MyJapi.f414bb1());
            assert (nf414bb + 1 == f414bb.asReadOnlyBuffer().get());

            out.println("\ncalling f515bb1(f415bb1())");
            final ByteBuffer f415bb = MyJapi.f415bb1().order(ByteOrder.nativeOrder());
            final short nf415bb = f415bb.asShortBuffer().get();
            MyJapi.f515bb1(f415bb);
            assert (nf415bb == f415bb.asShortBuffer().get());
            MyJapi.f515bb1(MyJapi.f415bb1());
            assert (nf415bb + 1 == f415bb.asShortBuffer().get());

            out.println("\ncalling f516bb1(f416bb1())");
            final ByteBuffer f416bb = MyJapi.f416bb1().order(ByteOrder.nativeOrder());
            final short nf416bb = f416bb.asShortBuffer().get();
            MyJapi.f516bb1(f416bb);
            assert (nf416bb == f416bb.asShortBuffer().get());
            MyJapi.f516bb1(MyJapi.f416bb1());
            assert (nf416bb + 1 == f416bb.asShortBuffer().get());

            out.println("\ncalling f517bb1(f417bb1())");
            final ByteBuffer f417bb = MyJapi.f417bb1().order(ByteOrder.nativeOrder());
            final int nf417bb = f417bb.asIntBuffer().get();
            MyJapi.f517bb1(f417bb);
            assert (nf417bb == f417bb.asIntBuffer().get());
            MyJapi.f517bb1(MyJapi.f417bb1());
            assert (nf417bb + 1 == f417bb.asIntBuffer().get());

            out.println("\ncalling f518bb1(f418bb1())");
            final ByteBuffer f418bb = MyJapi.f418bb1().order(ByteOrder.nativeOrder());
            final int nf418bb = f418bb.asIntBuffer().get();
            MyJapi.f518bb1(f418bb);
            assert (nf418bb == f418bb.asIntBuffer().get());
            MyJapi.f518bb1(MyJapi.f418bb1());
            assert (nf418bb + 1 == f418bb.asIntBuffer().get());

            out.println("\ncalling f521bb1(f421bb1())");
            final ByteBuffer f421bb = MyJapi.f421bb1().order(ByteOrder.nativeOrder());
            final long nf421bb = f421bb.asLongBuffer().get();
            MyJapi.f521bb1(f421bb);
            assert (nf421bb == f421bb.asLongBuffer().get());
            MyJapi.f521bb1(MyJapi.f421bb1());
            assert (nf421bb + 1 == f421bb.asLongBuffer().get());

            out.println("\ncalling f522bb1(f422bb1())");
            final ByteBuffer f422bb = MyJapi.f422bb1().order(ByteOrder.nativeOrder());
            final long nf422bb = f422bb.asLongBuffer().get();
            MyJapi.f522bb1(f422bb);
            assert (nf422bb == f422bb.asLongBuffer().get());
            MyJapi.f522bb1(MyJapi.f422bb1());
            assert (nf422bb + 1 == f422bb.asLongBuffer().get());

            out.println("\ncalling f523bb1(f423bb1())");
            final ByteBuffer f423bb = MyJapi.f423bb1().order(ByteOrder.nativeOrder());
            final float nf423bb = f423bb.asFloatBuffer().get();
            MyJapi.f523bb1(f423bb);
            assert (nf423bb == f423bb.asFloatBuffer().get());
            MyJapi.f523bb1(MyJapi.f423bb1());
            assert (nf423bb + 1 == f423bb.asFloatBuffer().get());

            out.println("\ncalling f524bb1(f424bb1())");
            final ByteBuffer f424bb = MyJapi.f424bb1().order(ByteOrder.nativeOrder());
            final double nf424bb = f424bb.asDoubleBuffer().get();
            MyJapi.f524bb1(f424bb);
            assert (nf424bb == f424bb.asDoubleBuffer().get());
            MyJapi.f524bb1(MyJapi.f424bb1());
            assert (nf424bb + 1 == f424bb.asDoubleBuffer().get());

            out.println("\ncalling f531bb1(f431bb1())");
            final ByteBuffer f431bb = MyJapi.f431bb1().order(ByteOrder.nativeOrder());
            final byte nf431bb = f431bb.asReadOnlyBuffer().get();
            MyJapi.f531bb1(f431bb);
            assert (nf431bb != f431bb.asReadOnlyBuffer().get());

            out.println("\ncalling f532bb1(f432bb1())");
            final ByteBuffer f432bb = MyJapi.f432bb1().order(ByteOrder.nativeOrder());
            final byte nf432bb = f432bb.asReadOnlyBuffer().get();
            MyJapi.f532bb1(f432bb);
            assert (nf432bb + 1 == f432bb.asReadOnlyBuffer().get());

            out.println("\ncalling f533bb1(f433bb1())");
            final ByteBuffer f433bb = MyJapi.f433bb1().order(ByteOrder.nativeOrder());
            final byte nf433bb = f433bb.asReadOnlyBuffer().get();
            MyJapi.f533bb1(f433bb);
            assert (nf433bb + 1 == f433bb.asReadOnlyBuffer().get());

            out.println("\ncalling f534bb1(f434bb1())");
            final ByteBuffer f434bb = MyJapi.f434bb1().order(ByteOrder.nativeOrder());
            final byte nf434bb = f434bb.asReadOnlyBuffer().get();
            MyJapi.f534bb1(f434bb);
            assert (nf434bb + 1 == f434bb.asReadOnlyBuffer().get());

            out.println("\ncalling f535bb1(f435bb1())");
            final ByteBuffer f435bb = MyJapi.f435bb1().order(ByteOrder.nativeOrder());
            final short nf435bb = f435bb.asShortBuffer().get();
            MyJapi.f535bb1(f435bb);
            assert (nf435bb + 1 == f435bb.asShortBuffer().get());

            out.println("\ncalling f536bb1(f436bb1())");
            final ByteBuffer f436bb = MyJapi.f436bb1().order(ByteOrder.nativeOrder());
            final short nf436bb = f436bb.asShortBuffer().get();
            MyJapi.f536bb1(f436bb);
            assert (nf436bb + 1 == f436bb.asShortBuffer().get());

            out.println("\ncalling f537bb1(f437bb1())");
            final ByteBuffer f437bb = MyJapi.f437bb1().order(ByteOrder.nativeOrder());
            final int nf437bb = f437bb.asIntBuffer().get();
            MyJapi.f537bb1(f437bb);
            assert (nf437bb + 1 == f437bb.asIntBuffer().get());

            out.println("\ncalling f538bb1(f438bb1())");
            final ByteBuffer f438bb = MyJapi.f438bb1().order(ByteOrder.nativeOrder());
            final int nf438bb = f438bb.asIntBuffer().get();
            MyJapi.f538bb1(f438bb);
            assert (nf438bb + 1 == f438bb.asIntBuffer().get());

            out.println("\ncalling f541bb1(f441bb1())");
            final ByteBuffer f441bb = MyJapi.f441bb1().order(ByteOrder.nativeOrder());
            final long nf441bb = f441bb.asLongBuffer().get();
            MyJapi.f541bb1(f441bb);
            assert (nf441bb + 1 == f441bb.asLongBuffer().get());

            out.println("\ncalling f542bb1(f442bb1())");
            final ByteBuffer f442bb = MyJapi.f442bb1().order(ByteOrder.nativeOrder());
            final long nf442bb = f442bb.asLongBuffer().get();
            MyJapi.f542bb1(f442bb);
            assert (nf442bb + 1 == f442bb.asLongBuffer().get());

            out.println("\ncalling f543bb1(f443bb1())");
            final ByteBuffer f443bb = MyJapi.f443bb1().order(ByteOrder.nativeOrder());
            final float nf443bb = f443bb.asFloatBuffer().get();
            MyJapi.f543bb1(f443bb);
            assert (nf443bb + 1 == f443bb.asFloatBuffer().get());

            out.println("\ncalling f544bb1(f444bb1())");
            final ByteBuffer f444bb = MyJapi.f444bb1().order(ByteOrder.nativeOrder());
            final double nf444bb = f444bb.asDoubleBuffer().get();
            MyJapi.f544bb1(f444bb);
            assert (nf444bb + 1 == f444bb.asDoubleBuffer().get());

            out.println("\ncalling f551bb1(f451bb1())");
            final ByteBuffer f451bb = MyJapi.f451bb1().order(ByteOrder.nativeOrder());
            final byte nf451bb = f451bb.asReadOnlyBuffer().get();
            MyJapi.f551bb1(f451bb);
            assert (nf451bb == f451bb.asReadOnlyBuffer().get());
            MyJapi.f551bb1(MyJapi.f451bb1());
            assert (nf451bb != f451bb.asReadOnlyBuffer().get());

            out.println("\ncalling f552bb1(f452bb1())");
            final ByteBuffer f452bb = MyJapi.f452bb1().order(ByteOrder.nativeOrder());
            final byte nf452bb = f452bb.asReadOnlyBuffer().get();
            MyJapi.f552bb1(f452bb);
            assert (nf452bb == f452bb.asReadOnlyBuffer().get());
            MyJapi.f552bb1(MyJapi.f452bb1());
            assert (nf452bb + 1 == f452bb.asReadOnlyBuffer().get());

            out.println("\ncalling f553bb1(f453bb1())");
            final ByteBuffer f453bb = MyJapi.f453bb1().order(ByteOrder.nativeOrder());
            final byte nf453bb = f453bb.asReadOnlyBuffer().get();
            MyJapi.f553bb1(f453bb);
            assert (nf453bb == f453bb.asReadOnlyBuffer().get());
            MyJapi.f553bb1(MyJapi.f453bb1());
            assert (nf453bb + 1 == f453bb.asReadOnlyBuffer().get());

            out.println("\ncalling f554bb1(f454bb1())");
            final ByteBuffer f454bb = MyJapi.f454bb1().order(ByteOrder.nativeOrder());
            final byte nf454bb = f454bb.asReadOnlyBuffer().get();
            MyJapi.f554bb1(f454bb);
            assert (nf454bb == f454bb.asReadOnlyBuffer().get());
            MyJapi.f554bb1(MyJapi.f454bb1());
            assert (nf454bb + 1 == f454bb.asReadOnlyBuffer().get());

            out.println("\ncalling f555bb1(f455bb1())");
            final ByteBuffer f455bb = MyJapi.f455bb1().order(ByteOrder.nativeOrder());
            final short nf455bb = f455bb.asShortBuffer().get();
            MyJapi.f555bb1(f455bb);
            assert (nf455bb == f455bb.asShortBuffer().get());
            MyJapi.f555bb1(MyJapi.f455bb1());
            assert (nf455bb + 1 == f455bb.asShortBuffer().get());

            out.println("\ncalling f556bb1(f456bb1())");
            final ByteBuffer f456bb = MyJapi.f456bb1().order(ByteOrder.nativeOrder());
            final short nf456bb = f456bb.asShortBuffer().get();
            MyJapi.f556bb1(f456bb);
            assert (nf456bb == f456bb.asShortBuffer().get());
            MyJapi.f556bb1(MyJapi.f456bb1());
            assert (nf456bb + 1 == f456bb.asShortBuffer().get());

            out.println("\ncalling f557bb1(f457bb1())");
            final ByteBuffer f457bb = MyJapi.f457bb1().order(ByteOrder.nativeOrder());
            final int nf457bb = f457bb.asIntBuffer().get();
            MyJapi.f557bb1(f457bb);
            assert (nf457bb == f457bb.asIntBuffer().get());
            MyJapi.f557bb1(MyJapi.f457bb1());
            assert (nf457bb + 1 == f457bb.asIntBuffer().get());

            out.println("\ncalling f558bb1(f458bb1())");
            final ByteBuffer f458bb = MyJapi.f458bb1().order(ByteOrder.nativeOrder());
            final int nf458bb = f458bb.asIntBuffer().get();
            MyJapi.f558bb1(f458bb);
            assert (nf458bb == f458bb.asIntBuffer().get());
            MyJapi.f558bb1(MyJapi.f458bb1());
            assert (nf458bb + 1 == f458bb.asIntBuffer().get());

            out.println("\ncalling f561bb1(f461bb1())");
            final ByteBuffer f461bb = MyJapi.f461bb1().order(ByteOrder.nativeOrder());
            final long nf461bb = f461bb.asLongBuffer().get();
            MyJapi.f561bb1(f461bb);
            assert (nf461bb == f461bb.asLongBuffer().get());
            MyJapi.f561bb1(MyJapi.f461bb1());
            assert (nf461bb + 1 == f461bb.asLongBuffer().get());

            out.println("\ncalling f562bb1(f462bb1())");
            final ByteBuffer f462bb = MyJapi.f462bb1().order(ByteOrder.nativeOrder());
            final long nf462bb = f462bb.asLongBuffer().get();
            MyJapi.f562bb1(f462bb);
            assert (nf462bb == f462bb.asLongBuffer().get());
            MyJapi.f562bb1(MyJapi.f462bb1());
            assert (nf462bb + 1 == f462bb.asLongBuffer().get());

            out.println("\ncalling f563bb1(f463bb1())");
            final ByteBuffer f463bb = MyJapi.f463bb1().order(ByteOrder.nativeOrder());
            final float nf463bb = f463bb.asFloatBuffer().get();
            MyJapi.f563bb1(f463bb);
            assert (nf463bb == f463bb.asFloatBuffer().get());
            MyJapi.f563bb1(MyJapi.f463bb1());
            assert (nf463bb + 1 == f463bb.asFloatBuffer().get());

            out.println("\ncalling f564bb1(f464bb1())");
            final ByteBuffer f464bb = MyJapi.f464bb1().order(ByteOrder.nativeOrder());
            final double nf464bb = f464bb.asDoubleBuffer().get();
            MyJapi.f564bb1(f464bb);
            assert (nf464bb == f464bb.asDoubleBuffer().get());
            MyJapi.f564bb1(MyJapi.f464bb1());
            assert (nf464bb + 1 == f464bb.asDoubleBuffer().get());

            out.println("\ncalling f571bb1(f471bb1())");
            final ByteBuffer f471bb = MyJapi.f471bb1().order(ByteOrder.nativeOrder());
            final byte nf471bb = f471bb.asReadOnlyBuffer().get();
            MyJapi.f571bb1(f471bb);
            assert (nf471bb != f471bb.asReadOnlyBuffer().get());

            out.println("\ncalling f572bb1(f472bb1())");
            final ByteBuffer f472bb = MyJapi.f472bb1().order(ByteOrder.nativeOrder());
            final byte nf472bb = f472bb.asReadOnlyBuffer().get();
            MyJapi.f572bb1(f472bb);
            assert (nf472bb + 1 == f472bb.asReadOnlyBuffer().get());

            out.println("\ncalling f573bb1(f473bb1())");
            final ByteBuffer f473bb = MyJapi.f473bb1().order(ByteOrder.nativeOrder());
            final byte nf473bb = f473bb.asReadOnlyBuffer().get();
            MyJapi.f573bb1(f473bb);
            assert (nf473bb + 1 == f473bb.asReadOnlyBuffer().get());

            out.println("\ncalling f574bb1(f474bb1())");
            final ByteBuffer f474bb = MyJapi.f474bb1().order(ByteOrder.nativeOrder());
            final byte nf474bb = f474bb.asReadOnlyBuffer().get();
            MyJapi.f574bb1(f474bb);
            assert (nf474bb + 1 == f474bb.asReadOnlyBuffer().get());

            out.println("\ncalling f575bb1(f475bb1())");
            final ByteBuffer f475bb = MyJapi.f475bb1().order(ByteOrder.nativeOrder());
            final short nf475bb = f475bb.asShortBuffer().get();
            MyJapi.f575bb1(f475bb);
            assert (nf475bb + 1 == f475bb.asShortBuffer().get());

            out.println("\ncalling f576bb1(f476bb1())");
            final ByteBuffer f476bb = MyJapi.f476bb1().order(ByteOrder.nativeOrder());
            final short nf476bb = f476bb.asShortBuffer().get();
            MyJapi.f576bb1(f476bb);
            assert (nf476bb + 1 == f476bb.asShortBuffer().get());

            out.println("\ncalling f577bb1(f477bb1())");
            final ByteBuffer f477bb = MyJapi.f477bb1().order(ByteOrder.nativeOrder());
            final int nf477bb = f477bb.asIntBuffer().get();
            MyJapi.f577bb1(f477bb);
            assert (nf477bb + 1 == f477bb.asIntBuffer().get());

            out.println("\ncalling f578bb1(f478bb1())");
            final ByteBuffer f478bb = MyJapi.f478bb1().order(ByteOrder.nativeOrder());
            final int nf478bb = f478bb.asIntBuffer().get();
            MyJapi.f578bb1(f478bb);
            assert (nf478bb + 1 == f478bb.asIntBuffer().get());

            out.println("\ncalling f581bb1(f481bb1())");
            final ByteBuffer f481bb = MyJapi.f481bb1().order(ByteOrder.nativeOrder());
            final long nf481bb = f481bb.asLongBuffer().get();
            MyJapi.f581bb1(f481bb);
            assert (nf481bb + 1 == f481bb.asLongBuffer().get());

            out.println("\ncalling f582bb1(f482bb1())");
            final ByteBuffer f482bb = MyJapi.f482bb1().order(ByteOrder.nativeOrder());
            final long nf482bb = f482bb.asLongBuffer().get();
            MyJapi.f582bb1(f482bb);
            assert (nf482bb + 1 == f482bb.asLongBuffer().get());

            out.println("\ncalling f583bb1(f483bb1())");
            final ByteBuffer f483bb = MyJapi.f483bb1().order(ByteOrder.nativeOrder());
            final float nf483bb = f483bb.asFloatBuffer().get();
            MyJapi.f583bb1(f483bb);
            assert (nf483bb + 1 == f483bb.asFloatBuffer().get());

            out.println("\ncalling f584bb1(f484bb1())");
            final ByteBuffer f484bb = MyJapi.f484bb1().order(ByteOrder.nativeOrder());
            final double nf484bb = f484bb.asDoubleBuffer().get();
            MyJapi.f584bb1(f484bb);
            assert (nf484bb + 1 == f484bb.asDoubleBuffer().get());
        }

        out.println();
        out.println("<-- MyJapiTest.test4bb1()");
    }

    static public void test4v1() {
        out.println("--> MyJapiTest.test4v1()");

        out.println("\ntesting array<size=1> mappings of pointers to primitive types");
        out.println("\ncalling MyJapi functions: f5xxv1(f4xxv1()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f437v1()");
            int[] a = MyJapi.f437v1();
            out.println("a = " + a);
            assert (a != null);
            out.println("a.length = " + a.length);
            assert (a.length == 1);
            out.println("a[0] = " + a[0]);
            final int nf437v1 = a[0];

            out.println("\ncalling f537v1()");
            MyJapi.f537v1(a);
            out.println("a = " + a);
            assert (a != null);
            out.println("a.length = " + a.length);
            assert (a.length == 1);
            out.println("a[0] = " + a[0]);
            assert (nf437v1 + 1 == a[0]);

            out.println("\ncalling f437v1()");
            a = MyJapi.f437v1();
            out.println("a = " + a);
            assert (a != null);
            out.println("a.length = " + a.length);
            assert (a.length == 1);
            out.println("a[0] = " + a[0]);
            assert (nf437v1 + 1 == a[0]);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f511v1(f411v1()); f511v1(f411v1())");
            final boolean[] nf411v0 = MyJapi.f411v1();
            MyJapi.f511v1(nf411v0);
            final boolean[] nf411v1 = MyJapi.f411v1();
            MyJapi.f511v1(nf411v1);
            assert (!nf411v0[0] == nf411v1[0]);

            out.println("\ncalling f512v1(f412v1()); f512v1(f412v1())");
            final byte[] nf412v0 = MyJapi.f412v1();
            MyJapi.f512v1(nf412v0);
            final byte[] nf412v1 = MyJapi.f412v1();
            MyJapi.f512v1(nf412v1);
            assert (nf412v0[0] + 1 == nf412v1[0]);

            out.println("\ncalling f513v1(f413v1()); f513v1(f413v1())");
            final byte[] nf413v0 = MyJapi.f413v1();
            MyJapi.f513v1(nf413v0);
            final byte[] nf413v1 = MyJapi.f413v1();
            MyJapi.f513v1(nf413v1);
            assert (nf413v0[0] + 1 == nf413v1[0]);

            out.println("\ncalling f514v1(f414v1()); f514v1(f414v1())");
            final byte[] nf414v0 = MyJapi.f414v1();
            MyJapi.f514v1(nf414v0);
            final byte[] nf414v1 = MyJapi.f414v1();
            MyJapi.f514v1(nf414v1);
            assert (nf414v0[0] + 1 == nf414v1[0]);

            out.println("\ncalling f515v1(f415v1()); f515v1(f415v1())");
            final short[] nf415v0 = MyJapi.f415v1();
            MyJapi.f515v1(nf415v0);
            final short[] nf415v1 = MyJapi.f415v1();
            MyJapi.f515v1(nf415v1);
            assert (nf415v0[0] + 1 == nf415v1[0]);

            out.println("\ncalling f516v1(f416v1()); f516v1(f416v1())");
            final short[] nf416v0 = MyJapi.f416v1();
            MyJapi.f516v1(nf416v0);
            final short[] nf416v1 = MyJapi.f416v1();
            MyJapi.f516v1(nf416v1);
            assert (nf416v0[0] + 1 == nf416v1[0]);

            out.println("\ncalling f517v1(f417v1()); f517v1(f417v1())");
            final int[] nf417v0 = MyJapi.f417v1();
            MyJapi.f517v1(nf417v0);
            final int[] nf417v1 = MyJapi.f417v1();
            MyJapi.f517v1(nf417v1);
            assert (nf417v0[0] + 1 == nf417v1[0]);

            out.println("\ncalling f518v1(f418v1()); f518v1(f418v1())");
            final int[] nf418v0 = MyJapi.f418v1();
            MyJapi.f518v1(nf418v0);
            final int[] nf418v1 = MyJapi.f418v1();
            MyJapi.f518v1(nf418v1);
            assert (nf418v0[0] + 1 == nf418v1[0]);

            out.println("\ncalling f521v1(f421v1()); f521v1(f421v1())");
            final long[] nf421v0 = MyJapi.f421v1();
            MyJapi.f521v1(nf421v0);
            final long[] nf421v1 = MyJapi.f421v1();
            MyJapi.f521v1(nf421v1);
            assert (nf421v0[0] + 1 == nf421v1[0]);

            out.println("\ncalling f522v1(f422v1()); f522v1(f422v1())");
            final long[] nf422v0 = MyJapi.f422v1();
            MyJapi.f522v1(nf422v0);
            final long[] nf422v1 = MyJapi.f422v1();
            MyJapi.f522v1(nf422v1);
            assert (nf422v0[0] + 1 == nf422v1[0]);

            out.println("\ncalling f523v1(f423v1()); f523v1(f423v1())");
            final float[] nf423v0 = MyJapi.f423v1();
            MyJapi.f523v1(nf423v0);
            final float[] nf423v1 = MyJapi.f423v1();
            MyJapi.f523v1(nf423v1);
            assert (nf423v0[0] + 1 == nf423v1[0]);

            out.println("\ncalling f524v1(f424v1()); f524v1(f424v1())");
            final double[] nf424v0 = MyJapi.f424v1();
            MyJapi.f524v1(nf424v0);
            final double[] nf424v1 = MyJapi.f424v1();
            MyJapi.f524v1(nf424v1);
            assert (nf424v0[0] + 1 == nf424v1[0]);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f531v1(f431v1()); f431v1()");
            final boolean[] nf431v0 = MyJapi.f431v1();
            MyJapi.f531v1(nf431v0);
            final boolean[] nf431v1 = MyJapi.f431v1();
            assert (nf431v1[0] == nf431v0[0]);

            out.println("\ncalling f532v1(f432v1()); f432v1()");
            final byte[] nf432v0 = MyJapi.f432v1();
            MyJapi.f532v1(nf432v0);
            final byte[] nf432v1 = MyJapi.f432v1();
            assert (nf432v1[0] == nf432v0[0]);

            out.println("\ncalling f533v1(f433v1()); f433v1()");
            final byte[] nf433v0 = MyJapi.f433v1();
            MyJapi.f533v1(nf433v0);
            final byte[] nf433v1 = MyJapi.f433v1();
            assert (nf433v1[0] == nf433v0[0]);

            out.println("\ncalling f534v1(f434v1()); f434v1()");
            final byte[] nf434v0 = MyJapi.f434v1();
            MyJapi.f534v1(nf434v0);
            final byte[] nf434v1 = MyJapi.f434v1();
            assert (nf434v1[0] == nf434v0[0]);

            out.println("\ncalling f535v1(f435v1()); f435v1()");
            final short[] nf435v0 = MyJapi.f435v1();
            MyJapi.f535v1(nf435v0);
            final short[] nf435v1 = MyJapi.f435v1();
            assert (nf435v1[0] == nf435v0[0]);

            out.println("\ncalling f536v1(f436v1()); f436v1()");
            final short[] nf436v0 = MyJapi.f436v1();
            MyJapi.f536v1(nf436v0);
            final short[] nf436v1 = MyJapi.f436v1();
            assert (nf436v1[0] == nf436v0[0]);

            out.println("\ncalling f537v1(f437v1()); f437v1()");
            final int[] nf437v0 = MyJapi.f437v1();
            MyJapi.f537v1(nf437v0);
            final int[] nf437v1 = MyJapi.f437v1();
            assert (nf437v1[0] == nf437v0[0]);

            out.println("\ncalling f538v1(f438v1()); f438v1()");
            final int[] nf438v0 = MyJapi.f438v1();
            MyJapi.f538v1(nf438v0);
            final int[] nf438v1 = MyJapi.f438v1();
            assert (nf438v1[0] == nf438v0[0]);

            out.println("\ncalling f541v1(f441v1()); f441v1()");
            final long[] nf441v0 = MyJapi.f441v1();
            MyJapi.f541v1(nf441v0);
            final long[] nf441v1 = MyJapi.f441v1();
            assert (nf441v1[0] == nf441v0[0]);

            out.println("\ncalling f542v1(f442v1()); f442v1()");
            final long[] nf442v0 = MyJapi.f442v1();
            MyJapi.f542v1(nf442v0);
            final long[] nf442v1 = MyJapi.f442v1();
            assert (nf442v1[0] == nf442v0[0]);

            out.println("\ncalling f543v1(f443v1()); f443v1()");
            final float[] nf443v0 = MyJapi.f443v1();
            MyJapi.f543v1(nf443v0);
            final float[] nf443v1 = MyJapi.f443v1();
            assert (nf443v1[0] == nf443v0[0]);

            out.println("\ncalling f544v1(f444v1()); f444v1()");
            final double[] nf444v0 = MyJapi.f444v1();
            MyJapi.f544v1(nf444v0);
            final double[] nf444v1 = MyJapi.f444v1();
            assert (nf444v1[0] == nf444v0[0]);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f551v1(f451v1()); f551v1(f451v1())");
            final boolean[] nf451v0 = MyJapi.f451v1();
            MyJapi.f551v1(nf451v0);
            final boolean[] nf451v1 = MyJapi.f451v1();
            MyJapi.f551v1(nf451v1);
            assert (!nf451v0[0] == nf451v1[0]);

            out.println("\ncalling f552v1(f452v1()); f552v1(f452v1())");
            final byte[] nf452v0 = MyJapi.f452v1();
            MyJapi.f552v1(nf452v0);
            final byte[] nf452v1 = MyJapi.f452v1();
            MyJapi.f552v1(nf452v1);
            assert (nf452v0[0] + 1 == nf452v1[0]);

            out.println("\ncalling f553v1(f453v1()); f553v1(f453v1())");
            final byte[] nf453v0 = MyJapi.f453v1();
            MyJapi.f553v1(nf453v0);
            final byte[] nf453v1 = MyJapi.f453v1();
            MyJapi.f553v1(nf453v1);
            assert (nf453v0[0] + 1 == nf453v1[0]);

            out.println("\ncalling f554v1(f454v1()); f554v1(f454v1())");
            final byte[] nf454v0 = MyJapi.f454v1();
            MyJapi.f554v1(nf454v0);
            final byte[] nf454v1 = MyJapi.f454v1();
            MyJapi.f554v1(nf454v1);
            assert (nf454v0[0] + 1 == nf454v1[0]);

            out.println("\ncalling f555v1(f455v1()); f555v1(f455v1())");
            final short[] nf455v0 = MyJapi.f455v1();
            MyJapi.f555v1(nf455v0);
            final short[] nf455v1 = MyJapi.f455v1();
            MyJapi.f555v1(nf455v1);
            assert (nf455v0[0] + 1 == nf455v1[0]);

            out.println("\ncalling f556v1(f456v1()); f556v1(f456v1())");
            final short[] nf456v0 = MyJapi.f456v1();
            MyJapi.f556v1(nf456v0);
            final short[] nf456v1 = MyJapi.f456v1();
            MyJapi.f556v1(nf456v1);
            assert (nf456v0[0] + 1 == nf456v1[0]);

            out.println("\ncalling f557v1(f457v1()); f557v1(f457v1())");
            final int[] nf457v0 = MyJapi.f457v1();
            MyJapi.f557v1(nf457v0);
            final int[] nf457v1 = MyJapi.f457v1();
            MyJapi.f557v1(nf457v1);
            assert (nf457v0[0] + 1 == nf457v1[0]);

            out.println("\ncalling f558v1(f458v1()); f558v1(f458v1())");
            final int[] nf458v0 = MyJapi.f458v1();
            MyJapi.f558v1(nf458v0);
            final int[] nf458v1 = MyJapi.f458v1();
            MyJapi.f558v1(nf458v1);
            assert (nf458v0[0] + 1 == nf458v1[0]);

            out.println("\ncalling f561v1(f461v1()); f561v1(f461v1())");
            final long[] nf461v0 = MyJapi.f461v1();
            MyJapi.f561v1(nf461v0);
            final long[] nf461v1 = MyJapi.f461v1();
            MyJapi.f561v1(nf461v1);
            assert (nf461v0[0] + 1 == nf461v1[0]);

            out.println("\ncalling f562v1(f462v1()); f562v1(f462v1())");
            final long[] nf462v0 = MyJapi.f462v1();
            MyJapi.f562v1(nf462v0);
            final long[] nf462v1 = MyJapi.f462v1();
            MyJapi.f562v1(nf462v1);
            assert (nf462v0[0] + 1 == nf462v1[0]);

            out.println("\ncalling f563v1(f463v1()); f563v1(f463v1())");
            final float[] nf463v0 = MyJapi.f463v1();
            MyJapi.f563v1(nf463v0);
            final float[] nf463v1 = MyJapi.f463v1();
            MyJapi.f563v1(nf463v1);
            assert (nf463v0[0] + 1 == nf463v1[0]);

            out.println("\ncalling f564v1(f464v1()); f564v1(f464v1())");
            final double[] nf464v0 = MyJapi.f464v1();
            MyJapi.f564v1(nf464v0);
            final double[] nf464v1 = MyJapi.f464v1();
            MyJapi.f564v1(nf464v1);
            assert (nf464v0[0] + 1 == nf464v1[0]);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f571v1(f471v1()); f471v1()");
            final boolean[] nf471v0 = MyJapi.f471v1();
            MyJapi.f571v1(nf471v0);
            final boolean[] nf471v1 = MyJapi.f471v1();
            assert (nf471v1[0] == nf471v0[0]);

            out.println("\ncalling f572v1(f472v1()); f472v1()");
            final byte[] nf472v0 = MyJapi.f472v1();
            MyJapi.f572v1(nf472v0);
            final byte[] nf472v1 = MyJapi.f472v1();
            assert (nf472v1[0] == nf472v0[0]);

            out.println("\ncalling f573v1(f473v1()); f473v1()");
            final byte[] nf473v0 = MyJapi.f473v1();
            MyJapi.f573v1(nf473v0);
            final byte[] nf473v1 = MyJapi.f473v1();
            assert (nf473v1[0] == nf473v0[0]);

            out.println("\ncalling f574v1(f474v1()); f474v1()");
            final byte[] nf474v0 = MyJapi.f474v1();
            MyJapi.f574v1(nf474v0);
            final byte[] nf474v1 = MyJapi.f474v1();
            assert (nf474v1[0] == nf474v0[0]);

            out.println("\ncalling f575v1(f475v1()); f475v1()");
            final short[] nf475v0 = MyJapi.f475v1();
            MyJapi.f575v1(nf475v0);
            final short[] nf475v1 = MyJapi.f475v1();
            assert (nf475v1[0] == nf475v0[0]);

            out.println("\ncalling f576v1(f476v1()); f476v1()");
            final short[] nf476v0 = MyJapi.f476v1();
            MyJapi.f576v1(nf476v0);
            final short[] nf476v1 = MyJapi.f476v1();
            assert (nf476v1[0] == nf476v0[0]);

            out.println("\ncalling f577v1(f477v1()); f477v1()");
            final int[] nf477v0 = MyJapi.f477v1();
            MyJapi.f577v1(nf477v0);
            final int[] nf477v1 = MyJapi.f477v1();
            assert (nf477v1[0] == nf477v0[0]);

            out.println("\ncalling f578v1(f478v1()); f478v1()");
            final int[] nf478v0 = MyJapi.f478v1();
            MyJapi.f578v1(nf478v0);
            final int[] nf478v1 = MyJapi.f478v1();
            assert (nf478v1[0] == nf478v0[0]);

            out.println("\ncalling f581v1(f481v1()); f481v1()");
            final long[] nf481v0 = MyJapi.f481v1();
            MyJapi.f581v1(nf481v0);
            final long[] nf481v1 = MyJapi.f481v1();
            assert (nf481v1[0] == nf481v0[0]);

            out.println("\ncalling f582v1(f482v1()); f482v1()");
            final long[] nf482v0 = MyJapi.f482v1();
            MyJapi.f582v1(nf482v0);
            final long[] nf482v1 = MyJapi.f482v1();
            assert (nf482v1[0] == nf482v0[0]);

            out.println("\ncalling f583v1(f483v1()); f483v1()");
            final float[] nf483v0 = MyJapi.f483v1();
            MyJapi.f583v1(nf483v0);
            final float[] nf483v1 = MyJapi.f483v1();
            assert (nf483v1[0] == nf483v0[0]);

            out.println("\ncalling f584v1(f484v1()); f484v1()");
            final double[] nf484v0 = MyJapi.f484v1();
            MyJapi.f584v1(nf484v0);
            final double[] nf484v1 = MyJapi.f484v1();
            assert (nf484v1[0] == nf484v0[0]);
        }

        out.println();
        out.println("<-- MyJapiTest.test4v1()");
    }

    static public void test5bb0() {
        out.println("--> MyJapiTest.test5bb0()");

        out.println("\ntesting nullable ByteBuffer<size=0> mappings of pointers to primitive types");
        out.println("\ncalling MyJapi functions: f7xxbb0(f6xxbb0()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException
        //   java/nio/ReadOnlyBufferException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f637bb0()");
            ByteBuffer bb = MyJapi.f637bb0();
            out.println("bb = " + bb);
            out.println("\ncalling f737bb0()");
            MyJapi.f737bb0(bb);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f711bb0(f611bb0())");
            MyJapi.f711bb0(MyJapi.f611bb0());

            out.println("\ncalling f712bb0(f612bb0())");
            MyJapi.f712bb0(MyJapi.f612bb0());

            out.println("\ncalling f713bb0(f613bb0())");
            MyJapi.f713bb0(MyJapi.f613bb0());

            out.println("\ncalling f714bb0(f614bb0())");
            MyJapi.f714bb0(MyJapi.f614bb0());

            out.println("\ncalling f715bb0(f615bb0())");
            MyJapi.f715bb0(MyJapi.f615bb0());

            out.println("\ncalling f716bb0(f616bb0())");
            MyJapi.f716bb0(MyJapi.f616bb0());

            out.println("\ncalling f717bb0(f617bb0())");
            MyJapi.f717bb0(MyJapi.f617bb0());

            out.println("\ncalling f718bb0(f618bb0())");
            MyJapi.f718bb0(MyJapi.f618bb0());

            out.println("\ncalling f721bb0(f621bb0())");
            MyJapi.f721bb0(MyJapi.f621bb0());

            out.println("\ncalling f722bb0(f622bb0())");
            MyJapi.f722bb0(MyJapi.f622bb0());

            out.println("\ncalling f723bb0(f623bb0())");
            MyJapi.f723bb0(MyJapi.f623bb0());

            out.println("\ncalling f724bb0(f624bb0())");
            MyJapi.f724bb0(MyJapi.f624bb0());

            out.println("\ncalling f731bb0(f631bb0())");
            MyJapi.f731bb0(MyJapi.f631bb0());

            out.println("\ncalling f732bb0(f632bb0())");
            MyJapi.f732bb0(MyJapi.f632bb0());

            out.println("\ncalling f733bb0(f633bb0())");
            MyJapi.f733bb0(MyJapi.f633bb0());

            out.println("\ncalling f734bb0(f634bb0())");
            MyJapi.f734bb0(MyJapi.f634bb0());

            out.println("\ncalling f735bb0(f635bb0())");
            MyJapi.f735bb0(MyJapi.f635bb0());

            out.println("\ncalling f736bb0(f636bb0())");
            MyJapi.f736bb0(MyJapi.f636bb0());

            out.println("\ncalling f737bb0(f637bb0())");
            MyJapi.f737bb0(MyJapi.f637bb0());

            out.println("\ncalling f738bb0(f638bb0())");
            MyJapi.f738bb0(MyJapi.f638bb0());

            out.println("\ncalling f741bb0(f641bb0())");
            MyJapi.f741bb0(MyJapi.f641bb0());

            out.println("\ncalling f742bb0(f642bb0())");
            MyJapi.f742bb0(MyJapi.f642bb0());

            out.println("\ncalling f743bb0(f643bb0())");
            MyJapi.f743bb0(MyJapi.f643bb0());

            out.println("\ncalling f744bb0(f644bb0())");
            MyJapi.f744bb0(MyJapi.f644bb0());

            out.println("\ncalling f751bb0(f651bb0())");
            MyJapi.f751bb0(MyJapi.f651bb0());

            out.println("\ncalling f752bb0(f652bb0())");
            MyJapi.f752bb0(MyJapi.f652bb0());

            out.println("\ncalling f753bb0(f653bb0())");
            MyJapi.f753bb0(MyJapi.f653bb0());

            out.println("\ncalling f754bb0(f654bb0())");
            MyJapi.f754bb0(MyJapi.f654bb0());

            out.println("\ncalling f755bb0(f655bb0())");
            MyJapi.f755bb0(MyJapi.f655bb0());

            out.println("\ncalling f756bb0(f656bb0())");
            MyJapi.f756bb0(MyJapi.f656bb0());

            out.println("\ncalling f757bb0(f657bb0())");
            MyJapi.f757bb0(MyJapi.f657bb0());

            out.println("\ncalling f758bb0(f658bb0())");
            MyJapi.f758bb0(MyJapi.f658bb0());

            out.println("\ncalling f761bb0(f661bb0())");
            MyJapi.f761bb0(MyJapi.f661bb0());

            out.println("\ncalling f762bb0(f662bb0())");
            MyJapi.f762bb0(MyJapi.f662bb0());

            out.println("\ncalling f763bb0(f663bb0())");
            MyJapi.f763bb0(MyJapi.f663bb0());

            out.println("\ncalling f764bb0(f664bb0())");
            MyJapi.f764bb0(MyJapi.f664bb0());

            out.println("\ncalling f771bb0(f671bb0())");
            MyJapi.f771bb0(MyJapi.f671bb0());

            out.println("\ncalling f772bb0(f672bb0())");
            MyJapi.f772bb0(MyJapi.f672bb0());

            out.println("\ncalling f773bb0(f673bb0())");
            MyJapi.f773bb0(MyJapi.f673bb0());

            out.println("\ncalling f774bb0(f674bb0())");
            MyJapi.f774bb0(MyJapi.f674bb0());

            out.println("\ncalling f775bb0(f675bb0())");
            MyJapi.f775bb0(MyJapi.f675bb0());

            out.println("\ncalling f776bb0(f676bb0())");
            MyJapi.f776bb0(MyJapi.f676bb0());

            out.println("\ncalling f777bb0(f677bb0())");
            MyJapi.f777bb0(MyJapi.f677bb0());

            out.println("\ncalling f778bb0(f678bb0())");
            MyJapi.f778bb0(MyJapi.f678bb0());

            out.println("\ncalling f781bb0(f681bb0())");
            MyJapi.f781bb0(MyJapi.f681bb0());

            out.println("\ncalling f782bb0(f682bb0())");
            MyJapi.f782bb0(MyJapi.f682bb0());

            out.println("\ncalling f783bb0(f683bb0())");
            MyJapi.f783bb0(MyJapi.f683bb0());

            out.println("\ncalling f784bb0(f684bb0())");
            MyJapi.f784bb0(MyJapi.f684bb0());
        }

        out.println();
        out.println("<-- MyJapiTest.test5bb0()");
    }

    static public void test5bb1() {
        out.println("--> MyJapiTest.test5bb1()");

        out.println("\ntesting nullable ByteBuffer<sizeof(C)> mappings of pointers to primitive types");
        out.println("\ncalling MyJapi functions: f7xxbb1(f6xxbb1()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException
        //   java/nio/ReadOnlyBufferException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f637bb1()");
            ByteBuffer bb = MyJapi.f637bb1();
            out.println("bb = " + bb);
            out.println("\ncalling f737bb1()");
            MyJapi.f737bb1(bb);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f711bb1(f611bb1())");
            MyJapi.f711bb1(MyJapi.f611bb1());

            out.println("\ncalling f712bb1(f612bb1())");
            MyJapi.f712bb1(MyJapi.f612bb1());

            out.println("\ncalling f713bb1(f613bb1())");
            MyJapi.f713bb1(MyJapi.f613bb1());

            out.println("\ncalling f714bb1(f614bb1())");
            MyJapi.f714bb1(MyJapi.f614bb1());

            out.println("\ncalling f715bb1(f615bb1())");
            MyJapi.f715bb1(MyJapi.f615bb1());

            out.println("\ncalling f716bb1(f616bb1())");
            MyJapi.f716bb1(MyJapi.f616bb1());

            out.println("\ncalling f717bb1(f617bb1())");
            MyJapi.f717bb1(MyJapi.f617bb1());

            out.println("\ncalling f718bb1(f618bb1())");
            MyJapi.f718bb1(MyJapi.f618bb1());

            out.println("\ncalling f721bb1(f621bb1())");
            MyJapi.f721bb1(MyJapi.f621bb1());

            out.println("\ncalling f722bb1(f622bb1())");
            MyJapi.f722bb1(MyJapi.f622bb1());

            out.println("\ncalling f723bb1(f623bb1())");
            MyJapi.f723bb1(MyJapi.f623bb1());

            out.println("\ncalling f724bb1(f624bb1())");
            MyJapi.f724bb1(MyJapi.f624bb1());

            out.println("\ncalling f731bb1(f631bb1())");
            MyJapi.f731bb1(MyJapi.f631bb1());

            out.println("\ncalling f732bb1(f632bb1())");
            MyJapi.f732bb1(MyJapi.f632bb1());

            out.println("\ncalling f733bb1(f633bb1())");
            MyJapi.f733bb1(MyJapi.f633bb1());

            out.println("\ncalling f734bb1(f634bb1())");
            MyJapi.f734bb1(MyJapi.f634bb1());

            out.println("\ncalling f735bb1(f635bb1())");
            MyJapi.f735bb1(MyJapi.f635bb1());

            out.println("\ncalling f736bb1(f636bb1())");
            MyJapi.f736bb1(MyJapi.f636bb1());

            out.println("\ncalling f737bb1(f637bb1())");
            MyJapi.f737bb1(MyJapi.f637bb1());

            out.println("\ncalling f738bb1(f638bb1())");
            MyJapi.f738bb1(MyJapi.f638bb1());

            out.println("\ncalling f741bb1(f641bb1())");
            MyJapi.f741bb1(MyJapi.f641bb1());

            out.println("\ncalling f742bb1(f642bb1())");
            MyJapi.f742bb1(MyJapi.f642bb1());

            out.println("\ncalling f743bb1(f643bb1())");
            MyJapi.f743bb1(MyJapi.f643bb1());

            out.println("\ncalling f744bb1(f644bb1())");
            MyJapi.f744bb1(MyJapi.f644bb1());

            out.println("\ncalling f751bb1(f651bb1())");
            MyJapi.f751bb1(MyJapi.f651bb1());

            out.println("\ncalling f752bb1(f652bb1())");
            MyJapi.f752bb1(MyJapi.f652bb1());

            out.println("\ncalling f753bb1(f653bb1())");
            MyJapi.f753bb1(MyJapi.f653bb1());

            out.println("\ncalling f754bb1(f654bb1())");
            MyJapi.f754bb1(MyJapi.f654bb1());

            out.println("\ncalling f755bb1(f655bb1())");
            MyJapi.f755bb1(MyJapi.f655bb1());

            out.println("\ncalling f756bb1(f656bb1())");
            MyJapi.f756bb1(MyJapi.f656bb1());

            out.println("\ncalling f757bb1(f657bb1())");
            MyJapi.f757bb1(MyJapi.f657bb1());

            out.println("\ncalling f758bb1(f658bb1())");
            MyJapi.f758bb1(MyJapi.f658bb1());

            out.println("\ncalling f761bb1(f661bb1())");
            MyJapi.f761bb1(MyJapi.f661bb1());

            out.println("\ncalling f762bb1(f662bb1())");
            MyJapi.f762bb1(MyJapi.f662bb1());

            out.println("\ncalling f763bb1(f663bb1())");
            MyJapi.f763bb1(MyJapi.f663bb1());

            out.println("\ncalling f764bb1(f664bb1())");
            MyJapi.f764bb1(MyJapi.f664bb1());

            out.println("\ncalling f771bb1(f671bb1())");
            MyJapi.f771bb1(MyJapi.f671bb1());

            out.println("\ncalling f772bb1(f672bb1())");
            MyJapi.f772bb1(MyJapi.f672bb1());

            out.println("\ncalling f773bb1(f673bb1())");
            MyJapi.f773bb1(MyJapi.f673bb1());

            out.println("\ncalling f774bb1(f674bb1())");
            MyJapi.f774bb1(MyJapi.f674bb1());

            out.println("\ncalling f775bb1(f675bb1())");
            MyJapi.f775bb1(MyJapi.f675bb1());

            out.println("\ncalling f776bb1(f676bb1())");
            MyJapi.f776bb1(MyJapi.f676bb1());

            out.println("\ncalling f777bb1(f677bb1())");
            MyJapi.f777bb1(MyJapi.f677bb1());

            out.println("\ncalling f778bb1(f678bb1())");
            MyJapi.f778bb1(MyJapi.f678bb1());

            out.println("\ncalling f781bb1(f681bb1())");
            MyJapi.f781bb1(MyJapi.f681bb1());

            out.println("\ncalling f782bb1(f682bb1())");
            MyJapi.f782bb1(MyJapi.f682bb1());

            out.println("\ncalling f783bb1(f683bb1())");
            MyJapi.f783bb1(MyJapi.f683bb1());

            out.println("\ncalling f784bb1(f684bb1())");
            MyJapi.f784bb1(MyJapi.f684bb1());
        }

        out.println();
        out.println("<-- MyJapiTest.test5bb1()");
    }


    static public void test5v0() {
        out.println("--> MyJapiTest.test5v0()");

        out.println("\ntesting nullable array<size=0> mappings of pointers to primitive types");
        out.println("\ncalling MyJapi functions: f7xxv0(f6xxv0()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f637v0()");
            int[] v = MyJapi.f637v0();
            out.println("v = " + v);
            out.println("\ncalling f737v0()");
            MyJapi.f737v0(v);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f711v0(f611v0())");
            MyJapi.f711v0(MyJapi.f611v0());

            out.println("\ncalling f712v0(f612v0())");
            MyJapi.f712v0(MyJapi.f612v0());

            out.println("\ncalling f713v0(f613v0())");
            MyJapi.f713v0(MyJapi.f613v0());

            out.println("\ncalling f714v0(f614v0())");
            MyJapi.f714v0(MyJapi.f614v0());

            out.println("\ncalling f715v0(f615v0())");
            MyJapi.f715v0(MyJapi.f615v0());

            out.println("\ncalling f716v0(f616v0())");
            MyJapi.f716v0(MyJapi.f616v0());

            out.println("\ncalling f717v0(f617v0())");
            MyJapi.f717v0(MyJapi.f617v0());

            out.println("\ncalling f718v0(f618v0())");
            MyJapi.f718v0(MyJapi.f618v0());

            out.println("\ncalling f721v0(f621v0())");
            MyJapi.f721v0(MyJapi.f621v0());

            out.println("\ncalling f722v0(f622v0())");
            MyJapi.f722v0(MyJapi.f622v0());

            out.println("\ncalling f723v0(f623v0())");
            MyJapi.f723v0(MyJapi.f623v0());

            out.println("\ncalling f724v0(f624v0())");
            MyJapi.f724v0(MyJapi.f624v0());

            out.println("\ncalling f731v0(f631v0())");
            MyJapi.f731v0(MyJapi.f631v0());

            out.println("\ncalling f732v0(f632v0())");
            MyJapi.f732v0(MyJapi.f632v0());

            out.println("\ncalling f733v0(f633v0())");
            MyJapi.f733v0(MyJapi.f633v0());

            out.println("\ncalling f734v0(f634v0())");
            MyJapi.f734v0(MyJapi.f634v0());

            out.println("\ncalling f735v0(f635v0())");
            MyJapi.f735v0(MyJapi.f635v0());

            out.println("\ncalling f736v0(f636v0())");
            MyJapi.f736v0(MyJapi.f636v0());

            out.println("\ncalling f737v0(f637v0())");
            MyJapi.f737v0(MyJapi.f637v0());

            out.println("\ncalling f738v0(f638v0())");
            MyJapi.f738v0(MyJapi.f638v0());

            out.println("\ncalling f741v0(f641v0())");
            MyJapi.f741v0(MyJapi.f641v0());

            out.println("\ncalling f742v0(f642v0())");
            MyJapi.f742v0(MyJapi.f642v0());

            out.println("\ncalling f743v0(f643v0())");
            MyJapi.f743v0(MyJapi.f643v0());

            out.println("\ncalling f744v0(f644v0())");
            MyJapi.f744v0(MyJapi.f644v0());

            out.println("\ncalling f751v0(f651v0())");
            MyJapi.f751v0(MyJapi.f651v0());

            out.println("\ncalling f752v0(f652v0())");
            MyJapi.f752v0(MyJapi.f652v0());

            out.println("\ncalling f753v0(f653v0())");
            MyJapi.f753v0(MyJapi.f653v0());

            out.println("\ncalling f754v0(f654v0())");
            MyJapi.f754v0(MyJapi.f654v0());

            out.println("\ncalling f755v0(f655v0())");
            MyJapi.f755v0(MyJapi.f655v0());

            out.println("\ncalling f756v0(f656v0())");
            MyJapi.f756v0(MyJapi.f656v0());

            out.println("\ncalling f757v0(f657v0())");
            MyJapi.f757v0(MyJapi.f657v0());

            out.println("\ncalling f758v0(f658v0())");
            MyJapi.f758v0(MyJapi.f658v0());

            out.println("\ncalling f761v0(f661v0())");
            MyJapi.f761v0(MyJapi.f661v0());

            out.println("\ncalling f762v0(f662v0())");
            MyJapi.f762v0(MyJapi.f662v0());

            out.println("\ncalling f763v0(f663v0())");
            MyJapi.f763v0(MyJapi.f663v0());

            out.println("\ncalling f764v0(f664v0())");
            MyJapi.f764v0(MyJapi.f664v0());

            out.println("\ncalling f771v0(f671v0())");
            MyJapi.f771v0(MyJapi.f671v0());

            out.println("\ncalling f772v0(f672v0())");
            MyJapi.f772v0(MyJapi.f672v0());

            out.println("\ncalling f773v0(f673v0())");
            MyJapi.f773v0(MyJapi.f673v0());

            out.println("\ncalling f774v0(f674v0())");
            MyJapi.f774v0(MyJapi.f674v0());

            out.println("\ncalling f775v0(f675v0())");
            MyJapi.f775v0(MyJapi.f675v0());

            out.println("\ncalling f776v0(f676v0())");
            MyJapi.f776v0(MyJapi.f676v0());

            out.println("\ncalling f777v0(f677v0())");
            MyJapi.f777v0(MyJapi.f677v0());

            out.println("\ncalling f778v0(f678v0())");
            MyJapi.f778v0(MyJapi.f678v0());

            out.println("\ncalling f781v0(f681v0())");
            MyJapi.f781v0(MyJapi.f681v0());

            out.println("\ncalling f782v0(f682v0())");
            MyJapi.f782v0(MyJapi.f682v0());

            out.println("\ncalling f783v0(f683v0())");
            MyJapi.f783v0(MyJapi.f683v0());

            out.println("\ncalling f784v0(f684v0())");
            MyJapi.f784v0(MyJapi.f684v0());
        }

        out.println();
        out.println("<-- MyJapiTest.test5v0()");
    }

    static public void test5v1() {
        out.println("--> MyJapiTest.test5v1()");

        out.println("\ntesting nullable array<size=1> mappings of pointers to primitive types");
        out.println("\ncalling MyJapi functions: f7xxv1(f6xxv1()) ...");

        // XXX todo: test for
        //   java/lang/IllegalArgumentException

        if (false) {
            // for testing/debugging
            out.println("\ncalling f637v1()");
            int[] v = MyJapi.f637v1();
            out.println("v = " + v);
            out.println("\ncalling f737v1()");
            MyJapi.f737v1(v);
        }

        for (int i = 0; i < 2; i++) {
            out.println("\ncalling f711v1(f611v1())");
            MyJapi.f711v1(MyJapi.f611v1());

            out.println("\ncalling f712v1(f612v1())");
            MyJapi.f712v1(MyJapi.f612v1());

            out.println("\ncalling f713v1(f613v1())");
            MyJapi.f713v1(MyJapi.f613v1());

            out.println("\ncalling f714v1(f614v1())");
            MyJapi.f714v1(MyJapi.f614v1());

            out.println("\ncalling f715v1(f615v1())");
            MyJapi.f715v1(MyJapi.f615v1());

            out.println("\ncalling f716v1(f616v1())");
            MyJapi.f716v1(MyJapi.f616v1());

            out.println("\ncalling f717v1(f617v1())");
            MyJapi.f717v1(MyJapi.f617v1());

            out.println("\ncalling f718v1(f618v1())");
            MyJapi.f718v1(MyJapi.f618v1());

            out.println("\ncalling f721v1(f621v1())");
            MyJapi.f721v1(MyJapi.f621v1());

            out.println("\ncalling f722v1(f622v1())");
            MyJapi.f722v1(MyJapi.f622v1());

            out.println("\ncalling f723v1(f623v1())");
            MyJapi.f723v1(MyJapi.f623v1());

            out.println("\ncalling f724v1(f624v1())");
            MyJapi.f724v1(MyJapi.f624v1());

            out.println("\ncalling f731v1(f631v1())");
            MyJapi.f731v1(MyJapi.f631v1());

            out.println("\ncalling f732v1(f632v1())");
            MyJapi.f732v1(MyJapi.f632v1());

            out.println("\ncalling f733v1(f633v1())");
            MyJapi.f733v1(MyJapi.f633v1());

            out.println("\ncalling f734v1(f634v1())");
            MyJapi.f734v1(MyJapi.f634v1());

            out.println("\ncalling f735v1(f635v1())");
            MyJapi.f735v1(MyJapi.f635v1());

            out.println("\ncalling f736v1(f636v1())");
            MyJapi.f736v1(MyJapi.f636v1());

            out.println("\ncalling f737v1(f637v1())");
            MyJapi.f737v1(MyJapi.f637v1());

            out.println("\ncalling f738v1(f638v1())");
            MyJapi.f738v1(MyJapi.f638v1());

            out.println("\ncalling f741v1(f641v1())");
            MyJapi.f741v1(MyJapi.f641v1());

            out.println("\ncalling f742v1(f642v1())");
            MyJapi.f742v1(MyJapi.f642v1());

            out.println("\ncalling f743v1(f643v1())");
            MyJapi.f743v1(MyJapi.f643v1());

            out.println("\ncalling f744v1(f644v1())");
            MyJapi.f744v1(MyJapi.f644v1());

            out.println("\ncalling f751v1(f651v1())");
            MyJapi.f751v1(MyJapi.f651v1());

            out.println("\ncalling f752v1(f652v1())");
            MyJapi.f752v1(MyJapi.f652v1());

            out.println("\ncalling f753v1(f653v1())");
            MyJapi.f753v1(MyJapi.f653v1());

            out.println("\ncalling f754v1(f654v1())");
            MyJapi.f754v1(MyJapi.f654v1());

            out.println("\ncalling f755v1(f655v1())");
            MyJapi.f755v1(MyJapi.f655v1());

            out.println("\ncalling f756v1(f656v1())");
            MyJapi.f756v1(MyJapi.f656v1());

            out.println("\ncalling f757v1(f657v1())");
            MyJapi.f757v1(MyJapi.f657v1());

            out.println("\ncalling f758v1(f658v1())");
            MyJapi.f758v1(MyJapi.f658v1());

            out.println("\ncalling f761v1(f661v1())");
            MyJapi.f761v1(MyJapi.f661v1());

            out.println("\ncalling f762v1(f662v1())");
            MyJapi.f762v1(MyJapi.f662v1());

            out.println("\ncalling f763v1(f663v1())");
            MyJapi.f763v1(MyJapi.f663v1());

            out.println("\ncalling f764v1(f664v1())");
            MyJapi.f764v1(MyJapi.f664v1());

            out.println("\ncalling f771v1(f671v1())");
            MyJapi.f771v1(MyJapi.f671v1());

            out.println("\ncalling f772v1(f672v1())");
            MyJapi.f772v1(MyJapi.f672v1());

            out.println("\ncalling f773v1(f673v1())");
            MyJapi.f773v1(MyJapi.f673v1());

            out.println("\ncalling f774v1(f674v1())");
            MyJapi.f774v1(MyJapi.f674v1());

            out.println("\ncalling f775v1(f675v1())");
            MyJapi.f775v1(MyJapi.f675v1());

            out.println("\ncalling f776v1(f676v1())");
            MyJapi.f776v1(MyJapi.f676v1());

            out.println("\ncalling f777v1(f677v1())");
            MyJapi.f777v1(MyJapi.f677v1());

            out.println("\ncalling f778v1(f678v1())");
            MyJapi.f778v1(MyJapi.f678v1());

            out.println("\ncalling f781v1(f681v1())");
            MyJapi.f781v1(MyJapi.f681v1());

            out.println("\ncalling f782v1(f682v1())");
            MyJapi.f782v1(MyJapi.f682v1());

            out.println("\ncalling f783v1(f683v1())");
            MyJapi.f783v1(MyJapi.f683v1());

            out.println("\ncalling f784v1(f684v1())");
            MyJapi.f784v1(MyJapi.f684v1());
        }

        out.println();
        out.println("<-- MyJapiTest.test5v1()");
    }

    static public void test6() {
        out.println("--> MyJapiTest.test6()");

        out.println("\ntesting object mappings: ...");
        int n = -1;

        out.println("\ncalling A.create_r()...");
        A a = A.create_r();
        out.println("... a = " + a);
        assert (a != null);

        out.println("\ncalling A.create_p()...");
        A ap = A.create_p();
        out.println("... a = " + ap);
        assert (a != null);

        out.println("\ncalling a.f0s()...");
        n = a.f0s();
        out.println("... a.f0s() = " + n);
        assert (n == 10);

        out.println("\ncalling A.f0n(a)...");
        n = A.f0n(a);
        out.println("... A.f0n(a) = " + n);
        assert (n == 11);

        out.println("\ncalling a.f0v1()...");
        n = a.f0v();
        out.println("... a.f0v() = " + n);
        assert (n == 12);

        out.println("\ncalling A.d0sc()...");
        n = A.d0sc();
        out.println("... A.d0sc() = " + n);
        assert (n == -10);

        out.println("\ncalling a.d0sc()...");
        n = a.d0sc();
        out.println("... a.d0sc() = " + n);
        assert (n == -10);

        out.println("\ncalling A.d0s(A.d0s() + 1)...");
        A.d0s(A.d0s() + 1);
        out.println("\n... calling A.d0s()...");
        n = A.d0s();
        out.println("... A.d0s() = " + n);
        assert (n == 11);

        out.println("\ncalling a.d0s(a.d0s() + 1)...");
        a.d0s(a.d0s() + 1);
        out.println("\n... calling a.d0s()...");
        n = a.d0s();
        out.println("... a.d0s() = " + n);
        assert (n == 12);

        out.println("\ncalling a.d0c()...");
        n = a.d0c();
        out.println("... a.d0c() = " + n);
        assert (n == -11);

        out.println("\ncalling a.d0(a.d0() + 1)...");
        a.d0(a.d0() + 1);
        out.println("\n... calling a.d0()...");
        n = a.d0();
        out.println("... a.d0() = " + n);
        assert (n == 12);

        out.println("\ncalling B0...");
        n = B0.f0s();
        out.println("... B0.f0s() = " + n);
        assert (n == 20);

        out.println("\ncalling a.newB0()...");
        B0 b0b0 = a.newB0();
        out.println("... b0b0 = " + b0b0);
        assert (b0b0 != null);

        out.println("\ncalling b0b0.f0s()...");
        n = b0b0.f0s();
        out.println("... b0b0.f0s() = " + n);
        assert (n == 20);

        out.println("\ncalling B0.f0n(b0b0)...");
        n = B0.f0n(b0b0);
        out.println("... B0.f0n(b0b0) = " + n);
        assert (n == 21);

        out.println("\ncalling b0b0.f0v()...");
        n = b0b0.f0v();
        out.println("... b0b0.f0v() = " + n);
        assert (n == 22);

        out.println("\ncalling B0.d0sc()...");
        n = B0.d0sc();
        out.println("... B0.d0sc() = " + n);
        assert (n == -20);

        out.println("\ncalling b0b0.d0sc()...");
        n = b0b0.d0sc();
        out.println("... b0b0.d0sc() = " + n);
        assert (n == -20);

        out.println("\ncalling B0.d0s(B0.d0s() + 1)...");
        B0.d0s(B0.d0s() + 1);
        out.println("\n... calling B0.d0s()...");
        n = B0.d0s();
        out.println("... B0.d0s() = " + n);
        assert (n == 21);

        out.println("\ncalling b0b0.d0s(b0b0.d0s() + 1)...");
        b0b0.d0s(b0b0.d0s() + 1);
        out.println("\n... calling b0b0.d0s()...");
        n = b0b0.d0s();
        out.println("... b0b0.d0s() = " + n);
        assert (n == 22);

        out.println("\ncalling B0.d0c(b0b0)...");
        n = B0.d0c(b0b0);
        out.println("... B0.d0c(b0b0) = " + n);
        assert (n == -21);

        out.println("\ncalling B0.d0(b0b0, B0.d0(b0b0) + 1)...");
        B0.d0(b0b0, B0.d0(b0b0) + 1);
        out.println("\n... calling B0.d0(b0b0)...");
        n = B0.d0(b0b0);
        out.println("... B0.d0(b0b0) = " + n);
        assert (n == 22);

        out.println("\ncalling a.del(b0b0)...");
        a.del(b0b0);

        out.println("\ncalling B1.f0s()...");
        n = B1.f0s();
        out.println("... B1.f0s() = " + n);
        assert (n == 30);

        out.println("\ncalling a.newB1()...");
        B1 b1b1 = a.newB1();
        B0 b0b1 = b1b1;
        out.println("... b0b1 = " + b0b1);
        assert (b0b1 != null);

        out.println("\ncalling b0b1.f0s()...");
        n = b0b1.f0s();
        out.println("... b0b1.f0s() = " + n);
        assert (n == 20);

        out.println("\ncalling B0.f0n(b0b1)...");
        n = B0.f0n(b0b1);
        out.println("... B0.f0n(b0b1) = " + n);
        assert (n == 21);

        out.println("\ncalling b0b1.f0v()...");
        n = b0b1.f0v();
        out.println("... b0b1.f0v() = " + n);
        assert (n == 32);

        out.println("\ncalling B1.d0sc()...");
        n = B1.d0sc();
        out.println("... B1.d0sc() = " + n);
        assert (n == -30);

        out.println("\ncalling b1b1.d0sc()...");
        n = b1b1.d0sc();
        out.println("... b1b1.d0sc() = " + n);
        assert (n == -30);

        out.println("\ncalling B1.d0s(B1.d0s() + 1)...");
        B1.d0s(B1.d0s() + 1);
        out.println("\n... calling B1.d0s()...");
        n = B1.d0s();
        out.println("... B1.d0s() = " + n);
        assert (n == 31);

        out.println("\ncalling b1b1.d0s(b1b1.d0s() + 1)...");
        b1b1.d0s(b1b1.d0s() + 1);
        out.println("\n... calling b1b1.d0s()...");
        n = b1b1.d0s();
        out.println("... b1b1.d0s() = " + n);
        assert (n == 32);

        out.println("\ncalling B1.d0c(b1b1)...");
        n = B1.d0c(b1b1);
        out.println("... B1.d0c(b1b1) = " + n);
        assert (n == -31);

        out.println("\ncalling B1.d0(b1b1, B1.d0(b1b1) + 1)...");
        B1.d0(b1b1, B1.d0(b1b1) + 1);
        out.println("\n... calling B1.d0(b1b1)...");
        n = B1.d0(b1b1);
        out.println("... B1.d0(b1b1) = " + n);
        assert (n == 32);

        out.println("\ncalling a.del(b1b1)...");
        a.del(b1b1);

        out.println("\ncalling A.print()...");
        A.print(a);

        out.println("\ncalling A.delete_r(a)...");
        A.delete_r(a);
        out.println("... a = " + a);

        out.println("\ncalling A.delete_p(ap)...");
        A.delete_p(ap);
        out.println("... ap = " + ap);

        out.println("\ncalling A.print()...");
        try {
            A.print(a);
            throw new RuntimeException("Expected exception not thrown.");
        } catch (AssertionError e) {
            out.println("... successfully caught: " + e);
        }
        out.println("\ncalling A.deliver_ptr()...");
        A pa = A.deliver_ptr();
        assert (pa != null);

        out.println("\ncalling A.take_ptr()...");
        A.take_ptr(pa);

        out.println("\ncalling A.deliver_null_ptr()...");
        A p0 = A.deliver_null_ptr();
        assert (p0 == null);

        out.println("\ncalling A.take_null_ptr()...");
        A.take_null_ptr(p0);

        out.println("\ncalling A.deliver_ref()...");
        A ra = A.deliver_ref();
        assert (ra != null);

        out.println("\ncalling A.take_ref()...");
        A.take_ref(ra);

        out.println("\ncalling A.deliver_null_ref()...");
        try {
            A.deliver_null_ref();
            throw new RuntimeException("Expected exception not thrown.");
        } catch (AssertionError e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling A.take_null_ref()...");
        try {
            A.take_null_ref(null);
            assert (false);
        } catch (IllegalArgumentException e) {
            out.println("... successfully caught: " + e);
        }

        out.println();
        out.println("<-- MyJapiTest.test6()");
    };

    static public void test7s() {
        out.println("--> MyJapiTest.test7s()");

        out.println("\ntesting String mappings");
        out.println("\ncalling MyJapi functions: s1x2(s0x2()) ...");

        out.println("\ncalling s112s(s012s())");
        MyJapi.s112s(MyJapi.s012s());

        out.println("\ncalling s112s(s052s())");
        MyJapi.s112s(MyJapi.s052s());

        out.println("\ncalling s112s(s032s())");
        MyJapi.s112s(MyJapi.s032s());

        out.println("\ncalling s112s(s072s())");
        MyJapi.s112s(MyJapi.s072s());

        out.println("\ncalling s152s(s012s())");
        MyJapi.s152s(MyJapi.s012s());

        out.println("\ncalling s152s(s052s())");
        MyJapi.s152s(MyJapi.s052s());

        out.println("\ncalling s152s(s032s())");
        MyJapi.s152s(MyJapi.s032s());

        out.println("\ncalling s152s(s072s())");
        MyJapi.s152s(MyJapi.s072s());

        out.println();
        out.println("<-- MyJapiTest.test7s()");
    };

    static public void test7bb0() {
        out.println("--> MyJapiTest.test7bb0()");

        out.println("\ntesting ByteBuffer<size=0> mappings of void/char pointers");
        out.println("\ncalling MyJapi functions: s1xxbb0(s0xxbb0()) ...");

        out.println("\ncalling s110bb0(s010bb0())");
        MyJapi.s110bb0(MyJapi.s010bb0());

        out.println("\ncalling s110bb0(s012bb0())");
        MyJapi.s110bb0(MyJapi.s012bb0());

        out.println("\ncalling s110bb0(s030bb0())");
        MyJapi.s110bb0(MyJapi.s030bb0());

        out.println("\ncalling s110bb0(s032bb0())");
        MyJapi.s110bb0(MyJapi.s032bb0());

        out.println("\ncalling s110bb0(s050bb0())");
        MyJapi.s110bb0(MyJapi.s050bb0());

        out.println("\ncalling s110bb0(s052bb0())");
        MyJapi.s110bb0(MyJapi.s052bb0());

        out.println("\ncalling s110bb0(s070bb0())");
        MyJapi.s110bb0(MyJapi.s070bb0());

        out.println("\ncalling s110bb0(s072bb0())");
        MyJapi.s110bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println("\ncalling s112bb0(s010bb0())");
        MyJapi.s112bb0(MyJapi.s010bb0());

        out.println("\ncalling s112bb0(s012bb0())");
        MyJapi.s112bb0(MyJapi.s012bb0());

        out.println("\ncalling s112bb0(s030bb0())");
        MyJapi.s112bb0(MyJapi.s030bb0());

        out.println("\ncalling s112bb0(s032bb0())");
        MyJapi.s112bb0(MyJapi.s032bb0());

        out.println("\ncalling s112bb0(s050bb0())");
        MyJapi.s112bb0(MyJapi.s050bb0());

        out.println("\ncalling s112bb0(s052bb0())");
        MyJapi.s112bb0(MyJapi.s052bb0());

        out.println("\ncalling s112bb0(s070bb0())");
        MyJapi.s112bb0(MyJapi.s070bb0());

        out.println("\ncalling s112bb0(s072bb0())");
        MyJapi.s112bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println("\ncalling s130bb0(s010bb0())");
        try {
            MyJapi.s130bb0(MyJapi.s010bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb0(s012bb0())");
        try {
            MyJapi.s130bb0(MyJapi.s012bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb0(s032bb0())");
        MyJapi.s130bb0(MyJapi.s032bb0());

        out.println("\ncalling s130bb0(s030bb0())");
        MyJapi.s130bb0(MyJapi.s030bb0());

        out.println("\ncalling s130bb0(s052bb0())");
        try {
            MyJapi.s130bb0(MyJapi.s052bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb0(s050bb0())");
        try {
            MyJapi.s130bb0(MyJapi.s050bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb0(s070bb0())");
        MyJapi.s130bb0(MyJapi.s070bb0());

        out.println("\ncalling s130bb0(s072bb0())");
        MyJapi.s130bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println("\ncalling s132bb0(s010bb0())");
        try {
            MyJapi.s132bb0(MyJapi.s010bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb0(s012bb0())");
        try {
            MyJapi.s132bb0(MyJapi.s012bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb0(s032bb0())");
        MyJapi.s132bb0(MyJapi.s032bb0());

        out.println("\ncalling s132bb0(s030bb0())");
        MyJapi.s132bb0(MyJapi.s030bb0());

        out.println("\ncalling s132bb0(s052bb0())");
        try {
            MyJapi.s132bb0(MyJapi.s052bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb0(s050bb0())");
        try {
            MyJapi.s132bb0(MyJapi.s050bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb0(s070bb0())");
        MyJapi.s132bb0(MyJapi.s070bb0());

        out.println("\ncalling s132bb0(s072bb0())");
        MyJapi.s132bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println("\ncalling s150bb0(s010bb0())");
        MyJapi.s150bb0(MyJapi.s010bb0());

        out.println("\ncalling s150bb0(s012bb0())");
        MyJapi.s150bb0(MyJapi.s012bb0());

        out.println("\ncalling s150bb0(s030bb0())");
        MyJapi.s150bb0(MyJapi.s030bb0());

        out.println("\ncalling s150bb0(s032bb0())");
        MyJapi.s150bb0(MyJapi.s032bb0());

        out.println("\ncalling s150bb0(s050bb0())");
        MyJapi.s150bb0(MyJapi.s050bb0());

        out.println("\ncalling s150bb0(s052bb0())");
        MyJapi.s150bb0(MyJapi.s052bb0());

        out.println("\ncalling s150bb0(s070bb0())");
        MyJapi.s150bb0(MyJapi.s070bb0());

        out.println("\ncalling s150bb0(s072bb0())");
        MyJapi.s150bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println("\ncalling s152bb0(s010bb0())");
        MyJapi.s152bb0(MyJapi.s010bb0());

        out.println("\ncalling s152bb0(s012bb0())");
        MyJapi.s152bb0(MyJapi.s012bb0());

        out.println("\ncalling s152bb0(s030bb0())");
        MyJapi.s152bb0(MyJapi.s030bb0());

        out.println("\ncalling s152bb0(s032bb0())");
        MyJapi.s152bb0(MyJapi.s032bb0());

        out.println("\ncalling s152bb0(s050bb0())");
        MyJapi.s152bb0(MyJapi.s050bb0());

        out.println("\ncalling s152bb0(s052bb0())");
        MyJapi.s152bb0(MyJapi.s052bb0());

        out.println("\ncalling s152bb0(s070bb0())");
        MyJapi.s152bb0(MyJapi.s070bb0());

        out.println("\ncalling s152bb0(s072bb0())");
        MyJapi.s152bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println("\ncalling s170bb0(s010bb0())");
        try {
            MyJapi.s170bb0(MyJapi.s010bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb0(s012bb0())");
        try {
            MyJapi.s170bb0(MyJapi.s012bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb0(s032bb0())");
        MyJapi.s170bb0(MyJapi.s032bb0());

        out.println("\ncalling s170bb0(s030bb0())");
        MyJapi.s170bb0(MyJapi.s030bb0());

        out.println("\ncalling s170bb0(s052bb0())");
        try {
            MyJapi.s170bb0(MyJapi.s052bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb0(s050bb0())");
        try {
            MyJapi.s170bb0(MyJapi.s050bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb0(s070bb0())");
        MyJapi.s170bb0(MyJapi.s070bb0());

        out.println("\ncalling s170bb0(s072bb0())");
        MyJapi.s170bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println("\ncalling s172bb0(s010bb0())");
        try {
            MyJapi.s172bb0(MyJapi.s010bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb0(s012bb0())");
        try {
            MyJapi.s172bb0(MyJapi.s012bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb0(s032bb0())");
        MyJapi.s172bb0(MyJapi.s032bb0());

        out.println("\ncalling s172bb0(s030bb0())");
        MyJapi.s172bb0(MyJapi.s030bb0());

        out.println("\ncalling s172bb0(s052bb0())");
        try {
            MyJapi.s172bb0(MyJapi.s052bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb0(s050bb0())");
        try {
            MyJapi.s172bb0(MyJapi.s050bb0());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb0(s070bb0())");
        MyJapi.s172bb0(MyJapi.s070bb0());

        out.println("\ncalling s172bb0(s072bb0())");
        MyJapi.s172bb0(MyJapi.s072bb0());

        // ------------------------------------------------------------

        out.println();
        out.println("<-- MyJapiTest.test7bb0()");
    };

    static public void test7bb1() {
        out.println("--> MyJapiTest.test7bb1()");

        out.println("\ntesting ByteBuffer<size=1> mappings of void/char pointers");
        out.println("\ncalling MyJapi functions: s1xxbb1(s0xxbb1()) ...");

        out.println("\ncalling s110bb1(s010bb1())");
        MyJapi.s110bb1(MyJapi.s010bb1());

        out.println("\ncalling s110bb1(s012bb1())");
        MyJapi.s110bb1(MyJapi.s012bb1());

        out.println("\ncalling s110bb1(s030bb1())");
        MyJapi.s110bb1(MyJapi.s030bb1());

        out.println("\ncalling s110bb1(s032bb1())");
        MyJapi.s110bb1(MyJapi.s032bb1());

        out.println("\ncalling s110bb1(s050bb1())");
        MyJapi.s110bb1(MyJapi.s050bb1());

        out.println("\ncalling s110bb1(s052bb1())");
        MyJapi.s110bb1(MyJapi.s052bb1());

        out.println("\ncalling s110bb1(s070bb1())");
        MyJapi.s110bb1(MyJapi.s070bb1());

        out.println("\ncalling s110bb1(s072bb1())");
        MyJapi.s110bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println("\ncalling s112bb1(s010bb1())");
        MyJapi.s112bb1(MyJapi.s010bb1());

        out.println("\ncalling s112bb1(s012bb1())");
        MyJapi.s112bb1(MyJapi.s012bb1());

        out.println("\ncalling s112bb1(s030bb1())");
        MyJapi.s112bb1(MyJapi.s030bb1());

        out.println("\ncalling s112bb1(s032bb1())");
        MyJapi.s112bb1(MyJapi.s032bb1());

        out.println("\ncalling s112bb1(s050bb1())");
        MyJapi.s112bb1(MyJapi.s050bb1());

        out.println("\ncalling s112bb1(s052bb1())");
        MyJapi.s112bb1(MyJapi.s052bb1());

        out.println("\ncalling s112bb1(s070bb1())");
        MyJapi.s112bb1(MyJapi.s070bb1());

        out.println("\ncalling s112bb1(s072bb1())");
        MyJapi.s112bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println("\ncalling s130bb1(s010bb1())");
        try {
            MyJapi.s130bb1(MyJapi.s010bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb1(s012bb1())");
        try {
            MyJapi.s130bb1(MyJapi.s012bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb1(s032bb1())");
        MyJapi.s130bb1(MyJapi.s032bb1());

        out.println("\ncalling s130bb1(s030bb1())");
        MyJapi.s130bb1(MyJapi.s030bb1());

        out.println("\ncalling s130bb1(s052bb1())");
        try {
            MyJapi.s130bb1(MyJapi.s052bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb1(s050bb1())");
        try {
            MyJapi.s130bb1(MyJapi.s050bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s130bb1(s070bb1())");
        MyJapi.s130bb1(MyJapi.s070bb1());

        out.println("\ncalling s130bb1(s072bb1())");
        MyJapi.s130bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println("\ncalling s132bb1(s010bb1())");
        try {
            MyJapi.s132bb1(MyJapi.s010bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb1(s012bb1())");
        try {
            MyJapi.s132bb1(MyJapi.s012bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb1(s032bb1())");
        MyJapi.s132bb1(MyJapi.s032bb1());

        out.println("\ncalling s132bb1(s030bb1())");
        MyJapi.s132bb1(MyJapi.s030bb1());

        out.println("\ncalling s132bb1(s052bb1())");
        try {
            MyJapi.s132bb1(MyJapi.s052bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb1(s050bb1())");
        try {
            MyJapi.s132bb1(MyJapi.s050bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s132bb1(s070bb1())");
        MyJapi.s132bb1(MyJapi.s070bb1());

        out.println("\ncalling s132bb1(s072bb1())");
        MyJapi.s132bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println("\ncalling s150bb1(s010bb1())");
        MyJapi.s150bb1(MyJapi.s010bb1());

        out.println("\ncalling s150bb1(s012bb1())");
        MyJapi.s150bb1(MyJapi.s012bb1());

        out.println("\ncalling s150bb1(s030bb1())");
        MyJapi.s150bb1(MyJapi.s030bb1());

        out.println("\ncalling s150bb1(s032bb1())");
        MyJapi.s150bb1(MyJapi.s032bb1());

        out.println("\ncalling s150bb1(s050bb1())");
        MyJapi.s150bb1(MyJapi.s050bb1());

        out.println("\ncalling s150bb1(s052bb1())");
        MyJapi.s150bb1(MyJapi.s052bb1());

        out.println("\ncalling s150bb1(s070bb1())");
        MyJapi.s150bb1(MyJapi.s070bb1());

        out.println("\ncalling s150bb1(s072bb1())");
        MyJapi.s150bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println("\ncalling s152bb1(s010bb1())");
        MyJapi.s152bb1(MyJapi.s010bb1());

        out.println("\ncalling s152bb1(s012bb1())");
        MyJapi.s152bb1(MyJapi.s012bb1());

        out.println("\ncalling s152bb1(s030bb1())");
        MyJapi.s152bb1(MyJapi.s030bb1());

        out.println("\ncalling s152bb1(s032bb1())");
        MyJapi.s152bb1(MyJapi.s032bb1());

        out.println("\ncalling s152bb1(s050bb1())");
        MyJapi.s152bb1(MyJapi.s050bb1());

        out.println("\ncalling s152bb1(s052bb1())");
        MyJapi.s152bb1(MyJapi.s052bb1());

        out.println("\ncalling s152bb1(s070bb1())");
        MyJapi.s152bb1(MyJapi.s070bb1());

        out.println("\ncalling s152bb1(s072bb1())");
        MyJapi.s152bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println("\ncalling s170bb1(s010bb1())");
        try {
            MyJapi.s170bb1(MyJapi.s010bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb1(s012bb1())");
        try {
            MyJapi.s170bb1(MyJapi.s012bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb1(s032bb1())");
        MyJapi.s170bb1(MyJapi.s032bb1());

        out.println("\ncalling s170bb1(s030bb1())");
        MyJapi.s170bb1(MyJapi.s030bb1());

        out.println("\ncalling s170bb1(s052bb1())");
        try {
            MyJapi.s170bb1(MyJapi.s052bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb1(s050bb1())");
        try {
            MyJapi.s170bb1(MyJapi.s050bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s170bb1(s070bb1())");
        MyJapi.s170bb1(MyJapi.s070bb1());

        out.println("\ncalling s170bb1(s072bb1())");
        MyJapi.s170bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println("\ncalling s172bb1(s010bb1())");
        try {
            MyJapi.s172bb1(MyJapi.s010bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb1(s012bb1())");
        try {
            MyJapi.s172bb1(MyJapi.s012bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb1(s032bb1())");
        MyJapi.s172bb1(MyJapi.s032bb1());

        out.println("\ncalling s172bb1(s030bb1())");
        MyJapi.s172bb1(MyJapi.s030bb1());

        out.println("\ncalling s172bb1(s052bb1())");
        try {
            MyJapi.s172bb1(MyJapi.s052bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb1(s050bb1())");
        try {
            MyJapi.s172bb1(MyJapi.s050bb1());
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s172bb1(s070bb1())");
        MyJapi.s172bb1(MyJapi.s070bb1());

        out.println("\ncalling s172bb1(s072bb1())");
        MyJapi.s172bb1(MyJapi.s072bb1());

        // ------------------------------------------------------------

        out.println();
        out.println("<-- MyJapiTest.test7bb1()");
    };

    static public void test7bb() {
        out.println("--> MyJapiTest.test7bb()");

        out.println("\ntesting ByteBuffer<size=0> mappings of null-allowed void/char pointers");
        out.println("\ncalling MyJapi functions: s3xxbb(s2xxbb()) ...");

        // ------------------------------------------------------------

        out.println("\ncalling s310bb(s210bb())");
        MyJapi.s310bb(MyJapi.s210bb());

        out.println("\ncalling s312bb(s212bb())");
        MyJapi.s312bb(MyJapi.s212bb());

        out.println("\ncalling s330bb(s230bb())");
        MyJapi.s330bb(MyJapi.s230bb());

        out.println("\ncalling s332bb(s232bb())");
        MyJapi.s332bb(MyJapi.s232bb());

        out.println("\ncalling s350bb(s250bb())");
        MyJapi.s350bb(MyJapi.s250bb());

        out.println("\ncalling s352bb(s252bb())");
        MyJapi.s352bb(MyJapi.s252bb());

        out.println("\ncalling s370bb(s270bb())");
        MyJapi.s370bb(MyJapi.s270bb());

        out.println("\ncalling s372bb(s272bb())");
        MyJapi.s372bb(MyJapi.s272bb());

        // ------------------------------------------------------------

        final ByteBuffer bbd0 = ByteBuffer.allocateDirect(0);
        out.println("\ncalling s310bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s310bb(bbd0);

        out.println("\ncalling s312bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s312bb(bbd0);

        out.println("\ncalling s330bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s330bb(bbd0);

        out.println("\ncalling s332bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s332bb(bbd0);

        out.println("\ncalling s350bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s350bb(bbd0);

        out.println("\ncalling s352bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s352bb(bbd0);

        out.println("\ncalling s370bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s370bb(bbd0);

        out.println("\ncalling s372bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s372bb(bbd0);

        // ------------------------------------------------------------

        final ByteBuffer bbd0ro = bbd0.asReadOnlyBuffer();
        out.println("\ncalling s310bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s310bb(bbd0ro);

        out.println("\ncalling s312bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s312bb(bbd0ro);

        out.println("\ncalling s330bb(ByteBuffer.allocateDirect(0))");
        try {
            MyJapi.s330bb(bbd0ro);
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s332bb(ByteBuffer.allocateDirect(0))");
        try {
            MyJapi.s332bb(bbd0ro);
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s350bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s350bb(bbd0ro);

        out.println("\ncalling s352bb(ByteBuffer.allocateDirect(0))");
        MyJapi.s352bb(bbd0ro);

        out.println("\ncalling s370bb(ByteBuffer.allocateDirect(0))");
        try {
            MyJapi.s370bb(bbd0ro);
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        out.println("\ncalling s372bb(ByteBuffer.allocateDirect(0))");
        try {
            MyJapi.s372bb(bbd0ro);
            assert (false);
        } catch (ReadOnlyBufferException e) {
            out.println("... successfully caught: " + e);
        }

        // ------------------------------------------------------------

        out.println();
        out.println("<-- MyJapiTest.test7bb()");
    };

    static public void test8() {
        out.println("--> MyJapiTest.test8()");

        out.println("\ntesting n-ary functions: g(), h() ...");
        int n = -1;

        out.println("\ncalling A.create_r(int)...");
        A a = A.create_r(5);
        out.println("... a = " + a);
        assert (a != null);

        out.println("\ncalling A.create_p(int)...");
        A ap = A.create_p(5);
        out.println("... ap = " + ap);
        assert (ap != null);

        out.println("\ncalling A.h0()...");
        A.h0();

        out.println("\ncalling A.h1(byte)...");
        A.h1((byte)1);

        out.println("\ncalling A.h2(byte), short)...");
        A.h2((byte)1, (short)2);

        out.println("\ncalling A.h3(byte), short, int)...");
        A.h3((byte)1, (short)2, (int)3);

        out.println("\ncalling A.h0r()...");
        n = A.h0r();
        assert (n == 0);

        out.println("\ncalling A.h1r(byte)...");
        n = A.h1r((byte)1);
        assert (n == 1);

        out.println("\ncalling A.h2r(byte, short)...");
        n = A.h2r((byte)1, (short)2);
        assert (n == 3);

        out.println("\ncalling A.h3r(byte, short, int)...");
        n = A.h3r((byte)1, (short)2, (int)3);
        assert (n == 6);

        out.println("\ncalling a.g0c()...");
        a.g0c();

        out.println("\ncalling a.g1c(byte)...");
        a.g1c((byte)1);

        out.println("\ncalling a.g2c(byte, short)...");
        a.g2c((byte)1, (short)2);

        out.println("\ncalling a.g3c(byte, short, int)...");
        a.g3c((byte)1, (short)2, (int)3);

        out.println("\ncalling a.g0()...");
        a.g0();

        out.println("\ncalling a.g1(byte)...");
        a.g1((byte)1);

        out.println("\ncalling a.g2(byte, short)...");
        a.g2((byte)1, (short)2);

        out.println("\ncalling a.g3(byte, short, int)...");
        a.g3((byte)1, (short)2, (int)3);

        out.println("\ncalling n = a.g0rc()...");
        n = a.g0rc();
        assert (n == 0);

        out.println("\ncalling n = a.g1rc(byte)...");
        n = a.g1rc((byte)1);
        assert (n == 1);

        out.println("\ncalling n = a.g2rc(byte, short)...");
        n = a.g2rc((byte)1, (short)2);
        assert (n == 3);

        out.println("\ncalling n = a.g3rc(byte, short, int)...");
        n = a.g3rc((byte)1, (short)2, (int)3);
        assert (n == 6);

        out.println("\ncalling n = a.g0r()...");
        n = a.g0r();
        assert (n == 0);

        out.println("\ncalling n = a.g1r(byte)...");
        n = a.g1r((byte)1);
        assert (n == 1);

        out.println("\ncalling n = a.g2r(byte, short)...");
        n = a.g2r((byte)1, (short)2);
        assert (n == 3);

        out.println("\ncalling n = a.g3r(byte, short, int)...");
        n = a.g3r((byte)1, (short)2, (int)3);
        assert (n == 6);

        out.println("\ncalling A.delete_p(a)...");
        A.delete_p(a);
        out.println("... a = " + a);

        out.println("\ncalling A.delete_r(ap)...");
        A.delete_r(ap);
        out.println("... ap = " + ap);

        out.println();
        out.println("<-- MyJapiTest.test8()");
    };

    static public void test9() {
        out.println("--> MyJapiTest.test9()");

        out.println("\ntesting const/inheritance mapping ...");

        // get C0, C1
        out.println("\ncalling C0.c() ...");
        final CI.C0 c0 = CI.C0.c();

        out.println("\ncalling C0.cc() ...");
        final CI.C0C c0c = CI.C0.cc();

        out.println("\ncalling C1.c() ...");
        final CI.C1 c1 = CI.C1.c();

        out.println("\ncalling C1.cc() ...");
        final CI.C1C c1c = CI.C1.cc();

        // for debugging
        if (false) {
            out.println("\nc0.print()...");
            c0.print();

            out.println("\nc0c.print()...");
            c0c.print();

            out.println("\nc1.print()...");
            c1.print();

            out.println("\nc1c.print()...");
            c1c.print();
        }

        // C0 -> C0
        out.println("\nc0c.take_C0Cp(c0c.deliver_C0Cp())...");
        c0c.take_C0Cp(c0c.deliver_C0Cp());

        out.println("\nc0c.take_C0Cr(c0c.deliver_C0Cr())...");
        c0c.take_C0Cr(c0c.deliver_C0Cr());

        out.println("\nc0c.take_C0Cp(c0.deliver_C0p())...");
        c0c.take_C0Cp(c0.deliver_C0p());

        out.println("\nc0c.take_C0Cr(c0.deliver_C0r())...");
        c0c.take_C0Cr(c0.deliver_C0r());

        out.println("\nc0.take_C0p(c0.deliver_C0p())...");
        c0.take_C0p(c0.deliver_C0p());

        out.println("\nc0.take_C0r(c0.deliver_C0r())...");
        c0.take_C0r(c0.deliver_C0r());

        // C1 -> C0
        out.println("\nc0c.take_C0Cp(c1c.deliver_C1Cp())...");
        c0c.take_C0Cp(c1c.deliver_C1Cp());

        out.println("\nc0c.take_C0Cr(c1c.deliver_C1Cr())...");
        c0c.take_C0Cr(c1c.deliver_C1Cr());

        out.println("\nc0c.take_C0Cp(c1.deliver_C1p())...");
        c0c.take_C0Cp(c1.deliver_C1p());

        out.println("\nc0c.take_C0Cr(c1.deliver_C1r())...");
        c0c.take_C0Cr(c1.deliver_C1r());

        out.println("\nc0.take_C0p(c1.deliver_C1p())...");
        c0.take_C0p(c1.deliver_C1p());

        out.println("\nc0.take_C0r(c1.deliver_C1r())...");
        c0.take_C0r(c1.deliver_C1r());

        // C1 -> C1
        out.println("\nc1c.take_C1Cp(c1c.deliver_C1Cp())...");
        c1c.take_C1Cp(c1c.deliver_C1Cp());

        out.println("\nc1c.take_C1Cr(c1c.deliver_C1Cr())...");
        c1c.take_C1Cr(c1c.deliver_C1Cr());

        out.println("\nc1c.take_C1Cp(c1.deliver_C1p())...");
        c1c.take_C1Cp(c1.deliver_C1p());

        out.println("\nc1c.take_C1Cr(c1.deliver_C1r())...");
        c1c.take_C1Cr(c1.deliver_C1r());

        out.println("\nc1.take_C1p(c1.deliver_C1p())...");
        c1.take_C1p(c1.deliver_C1p());

        out.println("\nc1.take_C1r(c1.deliver_C1r())...");
        c1.take_C1r(c1.deliver_C1r());

        out.println();
        out.println("<-- MyJapiTest.test9()");
    };

    static public long hash0(CI.C0CArray c0a, int n) {
        long r = 0;
        for (int i = 0; i < n; i++) {
            out.println("\ncalling CI.C0Array.at(" + i + ")...");
            final CI.C0C c0c = c0a.at(i);
            assert (c0c != null);

            out.println("\ncalling CI.C0C.id()...");
            r ^= c0c.id();
        }
        return r;
    }

    static public long hash1(CI.C1CArray c1a, int n) {
        long r = 0;
        for (int i = 0; i < n; i++) {
            out.println("\ncalling CI.C1Array.at(" + i + ")...");
            final CI.C1C c1c = c1a.at(i);
            assert (c1c != null);
            out.println("\ncalling CI.C1C.id()...");
            r ^= c1c.id();
        }
        return r;
    }

    static public void test10() {
        out.println("--> MyJapiTest.test10()");

        out.println("\ntesting object array functions ...");
        long r0, r1, r2;

        out.println("\ntesting object array of size 0...");
        {
            out.println("\ncalling CI.C0Array.create(0)...");
            final CI.C0Array c0a0 = CI.C0Array.create(0);
            assert (c0a0 != null);

            final CI.C0CArray c0a0c = c0a0;

            out.println("\ncalling CI.C0Array.delete(c0a0)...");
            CI.C0Array.delete(c0a0);
        }

        out.println("\ntesting non-const object array of size 1...");
        {
            final int n = 1;

            out.println("\ncalling CI.C0Array.create(" + n + ")...");
            final CI.C0Array c0a1 = CI.C0Array.create(n);
            assert (c0a1 != null);

            out.println("\ncalling CI.C0Array.at(0)...");
            final CI.C0 c01 = c0a1.at(0);
            assert (c01 != null);

            if (false) { // for debugging
                out.println("\ncalling CI.C0.print()...");
                c01.print();
            }

            out.println("\ncalling CI.C0.check(CI.C0.id())...");
            c01.check(c01.id());

            out.println("\ncalling hash(CI.C0Array, " + n + ")...");
            r0 = hash0(c0a1, n);

            out.println("\ncalling CI.C0.hash(CI.C0Array, " + n + ")...");
            r1 = CI.C0.hash(c0a1, n);
            assert (r1 == r0);

            out.println("\ncalling CI.C0.hash(CI.C0.pass(CI.C0Array), " + n + ")...");
            r2 = CI.C0.hash(CI.C0.pass(c0a1), n);
            assert (r2 == r0);

            out.println("\ntesting const object array of size " + n + "...");
            final CI.C0CArray c0ca1 = c0a1;

            out.println("\ncalling CI.C0Array.at(0)...");
            final CI.C0C c0c1 = c0ca1.at(0);
            assert (c0c1 != null);

            if (false) { // for debugging
                out.println("\ncalling CI.C0.print()...");
                c0c1.print();
            }

            out.println("\ncalling CI.C0C.check(CI.C0C.id())...");
            c0c1.check(c0c1.id());

            out.println("\ncalling hash(CI.C0CArray, " + n + ")...");
            r0 = hash0(c0ca1, n);

            out.println("\ncalling CI.C0.hash(CI.C0CArray, " + n + ")...");
            r1 = CI.C0.hash(c0ca1, n);
            assert (r1 == r0);

            out.println("\ncalling CI.C0.hash(CI.C0.pass(CI.C0CArray), " + n + ")...");
            r2 = CI.C0.hash(CI.C0.pass(c0ca1), n);
            assert (r2 == r0);

            out.println("\ncalling CI.C0Array.delete(c0a1)...");
            CI.C0Array.delete(c0a1);
        }

        out.println("\ntesting non-const object array of size >1...");
        {
            final int m = 3;

            out.println("\ncalling CI.C0Array.create(" + m + ")...");
            CI.C0Array c0a = CI.C0Array.create(m);
            assert (c0a != null);

            out.println("\ncalling hash(CI.C0Array, " + m + ")...");
            r0 = hash0(c0a, m);

            out.println("\ncalling CI.C0.hash(CI.C0Array, " + m + ")...");
            r1 = CI.C0.hash(c0a, m);
            assert (r1 == r0);

            out.println("\ncalling CI.C0.hash(CI.C0.pass(CI.C0Array), " + m + ")...");
            r2 = CI.C0.hash(CI.C0.pass(c0a), m);
            assert (r2 == r0);

            out.println("\ntesting const object array of size " + m + "...");
            final CI.C0CArray c0ca = c0a;

            out.println("\ncalling hash(CI.C0CArray, " + m + ")...");
            r0 = hash0(c0ca, m);

            out.println("\ncalling CI.C0.hash(CI.C0CArray, " + m + ")...");
            r1 = CI.C0.hash(c0ca, m);
            assert (r1 == r0);

            out.println("\ncalling CI.C0.hash(CI.C0.pass(CI.C0CArray), " + m + ")...");
            r2 = CI.C0.hash(CI.C0.pass(c0ca), m);
            assert (r2 == r0);

            out.println("\ncalling CI.C0Array.delete(c0a)...");
            CI.C0Array.delete(c0a);
        }

        out.println("\ntesting subclass object array of size 0...");
        {
            out.println("\ncalling CI.C1Array.create(0)...");
            final CI.C1Array c1a0 = CI.C1Array.create(0);
            assert (c1a0 != null);

            final CI.C1CArray c1a0c = c1a0;

            out.println("\ncalling CI.C1Array.delete(c1a0)...");
            CI.C1Array.delete(c1a0);
        }

        out.println("\ntesting subclass non-const object array of size 1...");
        {
            final int n = 1;

            out.println("\ncalling CI.C1Array.create(" + n + ")...");
            final CI.C1Array c1a1 = CI.C1Array.create(n);
            assert (c1a1 != null);

            out.println("\ncalling CI.C1Array.at(0)...");
            final CI.C1 c11 = c1a1.at(0);
            assert (c11 != null);

            if (false) { // for debugging
                out.println("\ncalling CI.C1.print()...");
                c11.print();
            }

            out.println("\ncalling CI.C1.check(CI.C1.id())...");
            c11.check(c11.id());

            out.println("\ncalling hash(CI.C1Array, " + n + ")...");
            r0 = hash1(c1a1, n);

            out.println("\ncalling CI.C1.hash(CI.C1Array, " + n + ")...");
            r1 = CI.C1.hash(c1a1, n);
            assert (r1 == r0);

            out.println("\ncalling CI.C1.hash(CI.C1.pass(CI.C1Array), " + n + ")...");
            r2 = CI.C1.hash(CI.C1.pass(c1a1), n);
            assert (r2 == r0);

            out.println("\ntesting subclass const object array of size " + n + "...");
            final CI.C1CArray c1ca1 = c1a1;

            out.println("\ncalling CI.C1Array.at(0)...");
            final CI.C1C c1c1 = c1ca1.at(0);
            assert (c1c1 != null);

            if (false) { // for debugging
                out.println("\ncalling CI.C1.print()...");
                c1c1.print();
            }

            out.println("\ncalling CI.C1C.check(CI.C1C.id())...");
            c1c1.check(c1c1.id());

            out.println("\ncalling hash(CI.C1CArray, " + n + ")...");
            r0 = hash1(c1ca1, n);

            out.println("\ncalling CI.C1.hash(CI.C1CArray, " + n + ")...");
            r1 = CI.C1.hash(c1ca1, n);
            assert (r1 == r0);

            out.println("\ncalling CI.C1.hash(CI.C1.pass(CI.C1CArray), " + n + ")...");
            r2 = CI.C1.hash(CI.C1.pass(c1ca1), n);
            assert (r2 == r0);

            out.println("\ncalling CI.C1Array.delete(c1a1)...");
            CI.C1Array.delete(c1a1);
        }

        out.println("\ntesting subclass non-const object array of size >1...");
        {
            final int m = 3;

            out.println("\ncalling CI.C1Array.create(" + m + ")...");
            CI.C1Array c1a = CI.C1Array.create(m);
            assert (c1a != null);

            out.println("\ncalling hash(CI.C1Array, " + m + ")...");
            r0 = hash1(c1a, m);

            out.println("\ncalling CI.C1.hash(CI.C1Array, " + m + ")...");
            r1 = CI.C1.hash(c1a, m);
            assert (r1 == r0);

            out.println("\ncalling CI.C1.hash(CI.C1.pass(CI.C1Array), " + m + ")...");
            r2 = CI.C1.hash(CI.C1.pass(c1a), m);
            assert (r2 == r0);

            out.println("\ntesting subclass const object array of size " + m + "...");
            final CI.C1CArray c1ca = c1a;

            out.println("\ncalling hash(CI.C1CArray, " + m + ")...");
            r0 = hash1(c1ca, m);

            out.println("\ncalling CI.C1.hash(CI.C1CArray, " + m + ")...");
            r1 = CI.C1.hash(c1ca, m);
            assert (r1 == r0);

            out.println("\ncalling CI.C1.hash(CI.C1.pass(CI.C1CArray), " + m + ")...");
            r2 = CI.C1.hash(CI.C1.pass(c1ca), m);
            assert (r2 == r0);

            out.println("\ncalling CI.C1Array.delete(c1a)...");
            CI.C1Array.delete(c1a);
        }

        out.println();
        out.println("<-- MyJapiTest.test10()");
    };

    static public void test11() {
        out.println("--> MyJapiTest.test11()");

        out.println("\ntesting function dispatch ...");

        out.println("\ncalling D0.sub().f_d0()...");
        assert (D0.sub().f_d0() == 20);

        out.println("\ncalling D1.f_nv(D0.sub())...");
        assert (D1.f_nv(D0.sub()) == 31);

        out.println("\ncalling D0.sub().f_v()...");
        assert (D0.sub().f_v() == 32);

        out.println("\ncalling D1.sub().f_d0()...");
        assert (D1.sub().f_d0() == 20);

        out.println("\ncalling D1.sub().f_d1()...");
        assert (D1.sub().f_d1() == 30);

        out.println("\ncalling D1.f_nv(D1.sub())...");
        assert (D1.f_nv(D1.sub()) == 31);

        out.println("\ncalling D1.sub().f_v()...");
        assert (D1.sub().f_v() == 42);

        out.println("\ncalling D2.sub()...");
        assert (D2.sub() == null);

        out.println();
        out.println("<-- MyJapiTest.test11()");
    };

    static public void test12() {
        out.println("--> MyJapiTest.test12()");

        out.println("\ntesting enum-int mapping ...");

        out.println("\nE.take_EE1(E.deliver_EE1())...");
        E.take_EE1(E.deliver_EE1());

        out.println("\nE.deliver_EE1()...");
        final int e = E.deliver_EE1();
        assert (e == E.EE1);

        out.println("\nE.take_EE1(e)...");
        E.take_EE1(e);

        out.println("\nE.take_EE1c(E.deliver_EE1c())...");
        E.take_EE1c(E.deliver_EE1c());

        out.println("\nE.deliver_EE1c()...");
        final int ec = E.deliver_EE1c();
        assert (ec == E.EE1);

        out.println("\nE.take_EE1c(ec)...");
        E.take_EE1c(ec);

        out.println();
        out.println("<-- MyJapiTest.test12()");
    };

    static public void test()
    {
        out.println("--> MyJapiTest.test()");

        final Class cls = MyJapiTest.class;
        out.println("    " + cls + " <" + cls.getClassLoader() + ">");

        // load native library
        out.println();
        loadSystemLibrary("myjapi");

        if (true) {
            out.println();
            test0();
            out.println();
            test1();
            out.println();
            test2();
            out.println();
            test3bb();
            out.println();
            test3v();
            out.println();
            test4bb0();
            out.println();
            test4bb1();
            out.println();
            test4v1();
            out.println();
            test5bb0();
            out.println();
            test5bb1();
            out.println();
            test5v0();
            out.println();
            test5v1();
            out.println();
            test6();
            out.println();
            test7s();
            out.println();
            test7bb0();
            out.println();
            test7bb1();
            out.println();
            test7bb();
            out.println();
            test8();
            out.println();
            test9();
            out.println();
            test10();
            out.println();
            test11();
            out.println();
            test12();
        } else {
            out.println();
            test12();
        }

        out.println();
        out.println("<-- MyJapiTest.test()");
    }


    static public void main(String[] args)
    {
        out.println("--> MyJapiTest.main()");

        out.println();
        test();

        out.println();
        out.println("<-- MyJapiTest.main()");
    }
}
