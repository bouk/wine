/*
 * oleacc tests
 *
 * Copyright 2008 Nikolay Sivov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define COBJMACROS

#include "wine/test.h"
#include <stdio.h>

#include "initguid.h"
#include <oleacc.h>

static void test_getroletext(void)
{
    INT ret, role;
    CHAR buf[2], *buff;
    WCHAR bufW[2], *buffW;

    /* wrong role number */
    ret = GetRoleTextA(-1, NULL, 0);
    ok(ret == 0, "GetRoleTextA doesn't return zero on wrong role number, got %d\n", ret);
    buf[0] = '*';
    ret = GetRoleTextA(-1, buf, 2);
    ok(ret == 0, "GetRoleTextA doesn't return zero on wrong role number, got %d\n", ret);
    ok(buf[0] == '*' ||
       broken(buf[0] == 0), /* Win98 and WinMe */
       "GetRoleTextA modified buffer on wrong role number\n");
    buf[0] = '*';
    ret = GetRoleTextA(-1, buf, 0);
    ok(ret == 0, "GetRoleTextA doesn't return zero on wrong role number, got %d\n", ret);
    ok(buf[0] == '*', "GetRoleTextA modified buffer on wrong role number\n");

    ret = GetRoleTextW(-1, NULL, 0);
    ok(ret == 0, "GetRoleTextW doesn't return zero on wrong role number, got %d\n", ret);
    bufW[0] = '*';
    ret = GetRoleTextW(-1, bufW, 2);
    ok(ret == 0, "GetRoleTextW doesn't return zero on wrong role number, got %d\n", ret);
    ok(bufW[0] == '\0' ||
       broken(bufW[0] == '*'), /* Win98 and WinMe */
       "GetRoleTextW doesn't return NULL char on wrong role number\n");
    bufW[0] = '*';
    ret = GetRoleTextW(-1, bufW, 0);
    ok(ret == 0, "GetRoleTextW doesn't return zero on wrong role number, got %d\n", ret);

    /* zero role number - not documented */
    ret = GetRoleTextA(0, NULL, 0);
    ok(ret > 0, "GetRoleTextA doesn't return (>0) for zero role number, got %d\n", ret);
    ret = GetRoleTextW(0, NULL, 0);
    ok(ret > 0, "GetRoleTextW doesn't return (>0) for zero role number, got %d\n", ret);

    /* NULL buffer, return length */
    ret = GetRoleTextA(ROLE_SYSTEM_TITLEBAR, NULL, 0);
    ok(ret > 0, "GetRoleTextA doesn't return length on NULL buffer, got %d\n", ret);
    ret = GetRoleTextA(ROLE_SYSTEM_TITLEBAR, NULL, 1);
    ok(ret > 0, "GetRoleTextA doesn't return length on NULL buffer, got %d\n", ret);
    ret = GetRoleTextW(ROLE_SYSTEM_TITLEBAR, NULL, 0);
    ok(ret > 0, "GetRoleTextW doesn't return length on NULL buffer, got %d\n", ret);
    ret = GetRoleTextW(ROLE_SYSTEM_TITLEBAR, NULL, 1);
    ok(ret > 0, "GetRoleTextW doesn't return length on NULL buffer, got %d\n", ret);

    /* use a smaller buffer */
    buf[0] = '*';
    ret = GetRoleTextA(ROLE_SYSTEM_TITLEBAR, buf, 1);
    ok(ret == 0, "GetRoleTextA returned wrong length\n");
    ok(buf[0] == '\0', "GetRoleTextA returned not zero-length buffer\n");
    buf[1] = '*';
    ret = GetRoleTextA(ROLE_SYSTEM_TITLEBAR, buf, 2);
    ok(ret == 1 ||
       ret == 0, /* Vista and W2K8 */
       "GetRoleTextA returned wrong length, got %d, expected 0 or 1\n", ret);
    if (ret == 1)
        ok(buf[1] == '\0', "GetRoleTextA returned not zero-length buffer : (%c)\n", buf[1]);

    bufW[0] = '*';
    ret = GetRoleTextW(ROLE_SYSTEM_TITLEBAR, bufW, 1);
    ok(ret == 0, "GetRoleTextW returned wrong length, got %d, expected 1\n", ret);
    ok(bufW[0] == '\0', "GetRoleTextW returned not zero-length buffer\n");
    bufW[1] = '*';
    ret = GetRoleTextW(ROLE_SYSTEM_TITLEBAR, bufW, 2);
    ok(ret == 1, "GetRoleTextW returned wrong length, got %d, expected 1\n", ret);
    ok(bufW[1] == '\0', "GetRoleTextW returned not zero-length buffer\n");

    /* use bigger buffer */
    ret = GetRoleTextA(ROLE_SYSTEM_TITLEBAR, NULL, 0);
    buff = HeapAlloc(GetProcessHeap(), 0, 2*ret);
    buff[2*ret-1] = '*';
    ret = GetRoleTextA(ROLE_SYSTEM_TITLEBAR, buff, 2*ret);
    ok(buff[2*ret-1] == '*', "GetRoleTextA shouldn't modify this part of buffer\n");
    HeapFree(GetProcessHeap(), 0, buff);

    ret = GetRoleTextW(ROLE_SYSTEM_TITLEBAR, NULL, 0);
    buffW = HeapAlloc(GetProcessHeap(), 0, 2*ret*sizeof(WCHAR));
    buffW[2*ret-1] = '*';
    ret = GetRoleTextW(ROLE_SYSTEM_TITLEBAR, buffW, 2*ret);
    ok(buffW[2*ret-1] == '*', "GetRoleTextW shouldn't modify this part of buffer\n");
    HeapFree(GetProcessHeap(), 0, buffW);

    /* check returned length for all roles */
    for(role = 0; role <= ROLE_SYSTEM_OUTLINEBUTTON; role++){
        CHAR buff2[100];
        WCHAR buff2W[100];

        /* NT4 and W2K don't clear the buffer on a nonexistent role in the A-call */
        memset(buff2, 0, sizeof(buff2));

        ret = GetRoleTextA(role, NULL, 0);
        /* Win98 up to W2K miss some of the roles */
        if (role >= ROLE_SYSTEM_SPLITBUTTON)
          ok(ret > 0 || broken(ret == 0), "Expected the role %d to be present\n", role);
        else
          ok(ret > 0, "Expected the role to be present\n");

        GetRoleTextA(role, buff2, sizeof(buff2));
        ok(ret == lstrlenA(buff2),
           "GetRoleTextA: returned length doesn't match returned buffer for role %d\n", role);

        /* Win98 and WinMe don't clear the buffer on a nonexistent role in the W-call */
        memset(buff2W, 0, sizeof(buff2W));

        ret = GetRoleTextW(role, NULL, 0);
        GetRoleTextW(role, buff2W, sizeof(buff2W)/sizeof(WCHAR));
        ok(ret == lstrlenW(buff2W),
           "GetRoleTextW: returned length doesn't match returned buffer for role %d\n", role);
    }
}

static int Object_ref = 1;
static HRESULT WINAPI Object_QueryInterface(IUnknown *iface, REFIID riid, void **ppv)
{
    if(IsEqualIID(riid, &IID_IUnknown)) {
        *ppv = iface;
        IUnknown_AddRef(iface);
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI Object_AddRef(IUnknown *iface)
{
    return InterlockedIncrement(&Object_ref);
}

static ULONG WINAPI Object_Release(IUnknown *iface)
{
    return InterlockedDecrement(&Object_ref);
}

static IUnknownVtbl ObjectVtbl = {
    Object_QueryInterface,
    Object_AddRef,
    Object_Release
};

static IUnknown Object = {&ObjectVtbl};

static void test_LresultFromObject(const char *name)
{
    PROCESS_INFORMATION proc;
    STARTUPINFOA startup;
    char cmdline[MAX_PATH];
    IUnknown *unk;
    HRESULT hres;
    LRESULT lres;

    lres = LresultFromObject(NULL, 0, 0);
    ok(lres == E_INVALIDARG, "got %lx\n", lres);

    hres = ObjectFromLresult(0, &IID_IUnknown, 0, (void**)&unk);
    ok(hres==MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,ERROR_INVALID_ADDRESS)
            || hres==E_FAIL, "got %x\n", hres);
    hres = ObjectFromLresult(0x10000, &IID_IUnknown, 0, (void**)&unk);
    ok(hres==MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,ERROR_INVALID_ADDRESS)
            || hres==E_FAIL, "got %x\n", hres);

    ok(Object_ref == 1, "Object_ref = %d\n", Object_ref);
    lres = LresultFromObject(&IID_IUnknown, 0, &Object);
    ok(SUCCEEDED(lres), "got %lx\n", lres);
    ok(Object_ref > 1, "Object_ref = %d\n", Object_ref);

    hres = ObjectFromLresult(lres, &IID_IUnknown, 0, (void**)&unk);
    ok(hres == S_OK, "hres = %x\n", hres);
    ok(unk == &Object, "unk != &Object\n");
    IUnknown_Release(unk);
    ok(Object_ref == 1, "Object_ref = %d\n", Object_ref);

    lres = LresultFromObject(&IID_IUnknown, 0, &Object);
    ok(SUCCEEDED(lres), "got %lx\n", lres);
    ok(Object_ref > 1, "Object_ref = %d\n", Object_ref);

    sprintf(cmdline, "\"%s\" main ObjectFromLresult %lx", name, lres);
    memset(&startup, 0, sizeof(startup));
    startup.cb = sizeof(startup);
    CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &startup, &proc);
    winetest_wait_child_process(proc.hProcess);
    ok(Object_ref == 1, "Object_ref = %d\n", Object_ref);
}

static LRESULT WINAPI test_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg) {
    case WM_GETOBJECT:
        if(lparam == OBJID_QUERYCLASSNAMEIDX) {
            ok(!wparam, "wparam = %lx\n", wparam);
            return 0;
        }

        ok(wparam==0xffffffff || broken(wparam==0x8000), "wparam = %lx\n", wparam);
        if(lparam == (DWORD)OBJID_CURSOR)
            return E_UNEXPECTED;
        if(lparam == (DWORD)OBJID_CLIENT)
            return LresultFromObject(&IID_IUnknown, wparam, &Object);

        ok(0, "unexpected (%ld)\n", lparam);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

static BOOL register_window_class(void)
{
    WNDCLASSA cls;

    memset(&cls, 0, sizeof(cls));
    cls.lpfnWndProc = test_window_proc;
    cls.lpszClassName = "oleacc_test";
    cls.hInstance = GetModuleHandleA(NULL);

    return RegisterClassA(&cls);
}

static void unregister_window_class(void)
{
    UnregisterClassA("oleacc_test", NULL);
}

static void test_AccessibleObjectFromWindow(void)
{
    IUnknown *unk;
    HRESULT hr;
    HWND hwnd;

    hr = AccessibleObjectFromWindow(NULL, OBJID_CURSOR, &IID_IUnknown, NULL);
    ok(hr == E_INVALIDARG, "got %x\n", hr);

    hr = AccessibleObjectFromWindow(NULL, OBJID_CURSOR, &IID_IUnknown, (void**)&unk);
    todo_wine ok(hr == S_OK, "got %x\n", hr);
    if(hr == S_OK) IUnknown_Release(unk);

    hwnd = CreateWindowA("oleacc_test", "test", WS_OVERLAPPEDWINDOW,
            0, 0, 0, 0, NULL, NULL, NULL, NULL);
    ok(hwnd != NULL, "CreateWindow failed\n");

    hr = AccessibleObjectFromWindow(hwnd, OBJID_CURSOR, &IID_IUnknown, (void**)&unk);
    ok(hr == E_UNEXPECTED, "got %x\n", hr);

    ok(Object_ref == 1, "Object_ref = %d\n", Object_ref);
    hr = AccessibleObjectFromWindow(hwnd, OBJID_CLIENT, &IID_IUnknown, (void**)&unk);
    ok(hr == S_OK, "got %x\n", hr);
    ok(Object_ref == 2, "Object_ref = %d\n", Object_ref);
    IUnknown_Release(unk);

    DestroyWindow(hwnd);
}

static void test_GetProcessHandleFromHwnd(void)
{
    HANDLE (WINAPI *pGetProcessHandleFromHwnd)(HWND);
    HANDLE proc;
    HWND hwnd;

    pGetProcessHandleFromHwnd = (void*)GetProcAddress(
            GetModuleHandleA("oleacc.dll"), "GetProcessHandleFromHwnd");
    if(!pGetProcessHandleFromHwnd) {
        win_skip("GetProcessHandleFromHwnd not available\n");
        return;
    }

    proc = pGetProcessHandleFromHwnd(NULL);
    ok(!proc, "proc = %p\n", proc);

    hwnd = CreateWindowA("static", "", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    ok(hwnd != NULL, "CreateWindow failed\n");

    proc = pGetProcessHandleFromHwnd(hwnd);
    ok(proc != NULL, "proc == NULL\n");
    CloseHandle(proc);

    DestroyWindow(hwnd);
}

static void test_default_client_accessible_object(void)
{
    static const WCHAR testW[] = {'t','e','s','t',' ','t',' ','&','j','u','n','k',0};
    static const WCHAR shortcutW[] = {'A','l','t','+','t',0};

    IAccessible *acc;
    HWND chld, hwnd;
    HRESULT hr;
    VARIANT vid, v;
    BSTR str;
    LONG l;

    hwnd = CreateWindowA("oleacc_test", "test &t &junk", WS_OVERLAPPEDWINDOW,
            0, 0, 0, 0, NULL, NULL, NULL, NULL);
    ok(hwnd != NULL, "CreateWindow failed\n");
    chld = CreateWindowA("static", "message", WS_CHILD,
            0, 0, 0, 0, hwnd, NULL, NULL, NULL);
    ok(chld != NULL, "CreateWindow failed\n");

    hr = CreateStdAccessibleObject(NULL, OBJID_CLIENT, &IID_IAccessible, (void**)&acc);
    ok(hr == E_FAIL, "got %x\n", hr);

    hr = CreateStdAccessibleObject(hwnd, OBJID_CLIENT, &IID_IAccessible, (void**)&acc);
    ok(hr == S_OK, "got %x\n", hr);

    hr = IAccessible_get_accChildCount(acc, &l);
    ok(hr == S_OK, "got %x\n", hr);
    ok(l == 1, "l = %d\n", l);

    V_VT(&vid) = VT_I4;
    V_I4(&vid) = CHILDID_SELF;
    hr = IAccessible_get_accName(acc, vid, &str);
    ok(hr == S_OK, "got %x\n", hr);
    ok(!lstrcmpW(str, testW), "name = %s\n", wine_dbgstr_w(str));
    SysFreeString(str);

    V_I4(&vid) = 1;
    str = (void*)0xdeadbeef;
    hr = IAccessible_get_accName(acc, vid, &str);
    ok(hr == E_INVALIDARG, "got %x\n", hr);
    ok(!str, "str != NULL\n");
    V_I4(&vid) = CHILDID_SELF;

    str = (void*)0xdeadbeef;
    hr = IAccessible_get_accValue(acc, vid, &str);
    ok(hr == S_FALSE, "got %x\n", hr);
    ok(!str, "str != NULL\n");

    str = (void*)0xdeadbeef;
    hr = IAccessible_get_accDescription(acc, vid, &str);
    ok(hr == S_FALSE, "got %x\n", hr);
    ok(!str, "str != NULL\n");

    V_VT(&v) = VT_DISPATCH;
    V_DISPATCH(&v) = (void*)0xdeadbeef;
    hr = IAccessible_get_accRole(acc, vid, &v);
    ok(hr == S_OK, "got %x\n", hr);
    ok(V_VT(&v) == VT_I4, "V_VT(&v) = %d\n", V_VT(&v));
    ok(V_I4(&v) == ROLE_SYSTEM_CLIENT, "V_I4(&v) = %d\n", V_I4(&v));

    V_VT(&v) = VT_DISPATCH;
    V_DISPATCH(&v) = (void*)0xdeadbeef;
    hr = IAccessible_get_accState(acc, vid, &v);
    ok(hr == S_OK, "got %x\n", hr);
    ok(V_VT(&v) == VT_I4, "V_VT(&v) = %d\n", V_VT(&v));
    ok(V_I4(&v) == (STATE_SYSTEM_FOCUSABLE|STATE_SYSTEM_INVISIBLE) ||
            broken(V_I4(&v) == STATE_SYSTEM_INVISIBLE), "V_I4(&v) = %x\n", V_I4(&v));

    str = (void*)0xdeadbeef;
    hr = IAccessible_get_accHelp(acc, vid, &str);
    ok(hr == S_FALSE, "got %x\n", hr);
    ok(!str, "str != NULL\n");

    hr = IAccessible_get_accKeyboardShortcut(acc, vid, &str);
    ok(hr == S_OK, "got %x\n", hr);
    ok(!lstrcmpW(str, shortcutW), "str = %s\n", wine_dbgstr_w(str));
    SysFreeString(str);

    str = (void*)0xdeadbeef;
    hr = IAccessible_get_accDefaultAction(acc, vid, &str);
    ok(hr == S_FALSE, "got %x\n", hr);
    ok(!str, "str != NULL\n");

    DestroyWindow(hwnd);

    hr = IAccessible_get_accChildCount(acc, &l);
    ok(hr == S_OK, "got %x\n", hr);
    ok(l == 0, "l = %d\n", l);

    hr = IAccessible_get_accName(acc, vid, &str);
    ok(hr == E_INVALIDARG, "got %x\n", hr);

    hr = IAccessible_get_accValue(acc, vid, &str);
    ok(hr == S_FALSE, "got %x\n", hr);

    hr = IAccessible_get_accRole(acc, vid, &v);
    ok(hr == S_OK, "got %x\n", hr);
    ok(V_VT(&v) == VT_I4, "V_VT(&v) = %d\n", V_VT(&v));
    ok(V_I4(&v) == ROLE_SYSTEM_CLIENT, "V_I4(&v) = %d\n", V_I4(&v));

    hr = IAccessible_get_accState(acc, vid, &v);
    ok(hr == S_OK, "got %x\n", hr);
    ok(V_VT(&v) == VT_I4, "V_VT(&v) = %d\n", V_VT(&v));
    ok(V_I4(&v) == STATE_SYSTEM_INVISIBLE, "V_I4(&v) = %x\n", V_I4(&v));

    IAccessible_Release(acc);
}

START_TEST(main)
{
    int argc;
    char **argv;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    argc = winetest_get_mainargs(&argv);
    if(argc == 4 && !strcmp(argv[2], "ObjectFromLresult")) {
        IUnknown *unk;
        HRESULT hres;
        LRESULT lres;

        sscanf(argv[3], "%lx", &lres);
        hres = ObjectFromLresult(lres, &IID_IUnknown, 0, (void**)&unk);
        ok(hres == S_OK, "hres = %x\n", hres);
        IUnknown_Release(unk);

        CoUninitialize();
        return;
    }

    if(!register_window_class()) {
        skip("can't register test window class\n");
        return;
    }

    test_getroletext();
    test_LresultFromObject(argv[0]);
    test_AccessibleObjectFromWindow();
    test_GetProcessHandleFromHwnd();
    test_default_client_accessible_object();

    unregister_window_class();
    CoUninitialize();
}
