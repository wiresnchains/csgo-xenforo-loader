#include "AntiDebug.hpp"

BOOL IsProcessRunning(DWORD pid)
{
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
    DWORD ret = WaitForSingleObject(process, 0);
    CloseHandle(process);
    return ret == WAIT_TIMEOUT;
}

bool found = true;

namespace protection {
    int check_process()
    {
        DWORD ida = helpers::GetProcId("ida.exe");
        DWORD ida64 = helpers::GetProcId("ida64.exe");
        DWORD ida32 = helpers::GetProcId("ida32.exe");
        DWORD ollydbg = helpers::GetProcId("ollydbg.exe");
        DWORD ollydbg64 = helpers::GetProcId("ollydbg64.exe");
        DWORD loaddll = helpers::GetProcId("loaddll.exe");
        DWORD httpdebugger = helpers::GetProcId("httpdebugger.exe");
        DWORD windowrenamer = helpers::GetProcId("windowrenamer.exe");
        DWORD processhacker = helpers::GetProcId("processhacker.exe");
        DWORD processhacker2 = helpers::GetProcId("Process Hacker.exe");
        DWORD processhacker3 = helpers::GetProcId("ProcessHacker.exe");
        DWORD HxD = helpers::GetProcId("HxD.exe");
        DWORD parsecd = helpers::GetProcId("parsecd.exe");
        DWORD dnSpy = helpers::GetProcId("dnSpy.exe");

        if (IsProcessRunning(ida) || IsProcessRunning(ida64) || IsProcessRunning(ida32) || IsProcessRunning(ollydbg) || IsProcessRunning(ollydbg64)
            || IsProcessRunning(loaddll) || IsProcessRunning(httpdebugger) || IsProcessRunning(windowrenamer) || IsProcessRunning(windowrenamer)
            || IsProcessRunning(processhacker) || IsProcessRunning(processhacker2) || IsProcessRunning(processhacker3) || IsProcessRunning(HxD)
            || IsProcessRunning(parsecd) || IsProcessRunning(dnSpy)) {
            return debug_results::being_debugged_peb;
        }
        else {
            return debug_results::none;
        }
    }

    int __cdecl vm_handler(EXCEPTION_RECORD* p_rec, void* est, unsigned char* p_context, void* disp)
    {
        found = true;
        (*(unsigned long*)(p_context + 0xB8)) += 4;
        return ExceptionContinueExecution;
    }

    void to_lower(unsigned char* input)
    {
        char* p = (char*)input;
        unsigned long length = strlen(p);
        for (unsigned long i = 0; i < length; i++) p[i] = tolower(p[i]);
    }

    const wchar_t* get_string(int index) {
        std::string value = "";

        switch (index) {
        case 0: value = xorstr_("Qt5QWindowIcon"); break;
        case 1: value = xorstr_("OLLYDBG"); break;
        case 2: value = xorstr_("SunAwtFrame"); break;
        case 3: value = xorstr_("ID"); break;
        case 4: value = xorstr_("ntdll.dll"); break;
        case 5: value = xorstr_("antidbg"); break;
        case 6: value = xorstr_("%random_environment_var_name_that_doesnt_exist?[]<>@\\;*!-{}#:/~%"); break;
        case 7: value = xorstr_("%random_file_name_that_doesnt_exist?[]<>@\\;*!-{}#:/~%"); break;
        }

        return std::wstring(value.begin(), value.end()).c_str();
    }

    int memory::being_debugged_peb() {
        BOOL found = FALSE;
        _asm
        {
            xor eax, eax;			//clear the eax register
            mov eax, fs: [0x30] ;	//reference start of the process environment block
            mov eax, [eax + 0x02];	//beingdebugged is stored in peb + 2
            and eax, 0x000000FF;	//reference one byte
            mov found, eax;			//copy value to found
        }

        return (found) ? debug_results::being_debugged_peb : debug_results::none;
    }

    int memory::remote_debugger_present() {
        //declare variables to hold the process handle & bool to check if it was found
        HANDLE h_process = INVALID_HANDLE_VALUE;
        BOOL found = FALSE;

        //set the process handle to the current process
        h_process = GetCurrentProcess();
        //check if a remote debugger is present
        CheckRemoteDebuggerPresent(h_process, &found);

        //if found is true, we return the right code.
        return (found) ? debug_results::remote_debugger_present : debug_results::none;
    }

    int memory::check_window_name() {
        const wchar_t* names[4] = { get_string(0), get_string(1), get_string(2), get_string(3) };

        for (const wchar_t* name : names) {
            if (FindWindow((LPCSTR)name, 0)) { return debug_results::find_window; }
        }

        return debug_results::none;
    }

    int memory::is_debugger_present() {
        //if debugger is found, we return the right code.
        return (IsDebuggerPresent()) ? debug_results::debugger_is_present : debug_results::none;
    }

    int memory::nt_global_flag_peb() {
        //bool to indicate find status
        BOOL found = FALSE;
        _asm
        {
            xor eax, eax;			//clear the eax register
            mov eax, fs: [0x30] ;   //reference start of the peb
            mov eax, [eax + 0x68];	//peb+0x68 points to NtGlobalFlags
            and eax, 0x00000070;	//check three flags
            mov found, eax;			//copy value to found
        }

        //if found is true, we return the right code.
        return (found) ? debug_results::being_debugged_peb : debug_results::none;
    }

    int memory::nt_query_information_process() {
        HANDLE h_process = INVALID_HANDLE_VALUE;
        DWORD found = FALSE;
        DWORD process_debug_port = 0x07;	//first method, check msdn for details
        DWORD process_debug_flags = 0x1F;	//second method, check msdn for details

        //get a handle to ntdll.dll so we can use NtQueryInformationProcess
        HMODULE h_ntdll = LoadLibraryW(get_string(4));

        //if we cant get the handle for some reason, we return none
        if (h_ntdll == INVALID_HANDLE_VALUE || h_ntdll == NULL) { return debug_results::none; }

        //dynamically acquire the address of NtQueryInformationProcess
        _NtQueryInformationProcess NtQueryInformationProcess = NULL;
        NtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(h_ntdll, xorstr_("NtQueryInformationProcess"));

        //if we cant get access for some reason, we return none
        if (NtQueryInformationProcess == NULL) { return debug_results::none; }

        //method 1: query ProcessDebugPort
        h_process = GetCurrentProcess();
        NTSTATUS status = NtQueryInformationProcess(h_process, ProcessDebugPort, &found, sizeof(DWORD), NULL);

        //found something
        if (!status && found) { return debug_results::nt_query_information_process; }

        //method 2: query ProcessDebugFlags
        status = NtQueryInformationProcess(h_process, process_debug_flags, &found, sizeof(DWORD), NULL);

        //the ProcessDebugFlags set found to 1 if no debugger is found, so we check !found.
        if (!status && !found) { return debug_results::nt_query_information_process; }

        return debug_results::none;
    }

    int memory::nt_set_information_thread() {
        DWORD thread_hide_from_debugger = 0x11;

        //get a handle to ntdll.dll so we can use NtQueryInformationProcess
        HMODULE h_ntdll = LoadLibraryW(get_string(4));

        //if we cant get the handle for some reason, we return none
        if (h_ntdll == INVALID_HANDLE_VALUE || h_ntdll == NULL) { return debug_results::none; }

        //dynamically acquire the address of NtQueryInformationProcess
        _NtQueryInformationProcess NtQueryInformationProcess = NULL;
        NtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(h_ntdll, xorstr_("NtQueryInformationProcess"));

        //if we cant get access for some reason, we return none
        if (NtQueryInformationProcess == NULL) { return debug_results::none; }

        //make call to deattach a debugger :moyai:
        (_NtSetInformationThread)(GetCurrentThread(), thread_hide_from_debugger, 0, 0, 0);

        return debug_results::none;
    }

    int memory::debug_active_process() {
        BOOL found = FALSE;
        STARTUPINFOA si = { 0 };
        PROCESS_INFORMATION pi = { 0 };
        si.cb = sizeof(si);
        TCHAR sz_path[MAX_PATH];
        DWORD exit_code = 0;

        DWORD proc_id = GetCurrentProcessId();
        std::stringstream stream;
        stream << proc_id;
        std::string args = stream.str();

        const char* cp_id = args.c_str();
        CreateMutex(NULL, FALSE, (LPCSTR)get_string(5));
        if (GetLastError() != ERROR_SUCCESS)
        {
            //if we get here, we're in the child process
            if (DebugActiveProcess((DWORD)atoi(cp_id)))
            {
                //no debugger found
                return debug_results::none;
            }
            else
            {
                //debugger found, exit child with unique code that we can check for
                exit(555);
            }
        }

        //parent process
        DWORD pid = GetCurrentProcessId();
        GetModuleFileName(NULL, sz_path, MAX_PATH);

        char cmdline[MAX_PATH + 1 + sizeof(int)];
        snprintf(cmdline, sizeof(cmdline), xorstr_("%ws %d"), sz_path, pid);

        //start child process
        BOOL success = CreateProcessA(
            NULL,		//path (NULL means use cmdline instead)
            cmdline,	//command line
            NULL,		//process handle not inheritable
            NULL,		//thread handle not inheritable
            FALSE,		//set handle inheritance to FALSE
            0,			//no creation flags
            NULL,		//use parent's environment block
            NULL,		//use parent's starting directory 
            &si,		//pointer to STARTUPINFO structure
            &pi);		//pointer to PROCESS_INFORMATION structure

        //wait until child process exits and get the code
        WaitForSingleObject(pi.hProcess, INFINITE);

        //check for our unique exit code
        if (GetExitCodeProcess(pi.hProcess, &exit_code) == 555) { found = TRUE; }

        // Close process and thread handles. 
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        //if found is true, we return the right code.
        return (found) ? debug_results::being_debugged_peb : debug_results::none;
    }

    int memory::write_buffer() {
        //first option

        //vars to store the amount of accesses to the buffer and the granularity for GetWriteWatch()
        ULONG_PTR hits;
        DWORD granularity;

        PVOID* addresses = static_cast<PVOID*>(VirtualAlloc(NULL, 4096 * sizeof(PVOID), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
        if (addresses == NULL) {
            return debug_results::write_buffer;
        }

        int* buffer = static_cast<int*>(VirtualAlloc(NULL, 4096 * 4096, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, PAGE_READWRITE));
        if (buffer == NULL) {
            VirtualFree(addresses, 0, MEM_RELEASE);
            return debug_results::write_buffer;
        }

        //read the buffer once
        buffer[0] = 1234;

        hits = 4096;
        if (GetWriteWatch(0, buffer, 4096, addresses, &hits, &granularity) != 0) { return debug_results::write_buffer; }
        else
        {
            //free the memory again
            VirtualFree(addresses, 0, MEM_RELEASE);
            VirtualFree(buffer, 0, MEM_RELEASE);

            //we should have 1 hit if everything is fine
            return (hits == 1) ? debug_results::none : debug_results::write_buffer;
        }

        //second option

        BOOL result = FALSE, error = FALSE;

        addresses = static_cast<PVOID*>(VirtualAlloc(NULL, 4096 * sizeof(PVOID), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
        if (addresses == NULL) { return debug_results::write_buffer; }

        buffer = static_cast<int*>(VirtualAlloc(NULL, 4096 * 4096, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, PAGE_READWRITE));
        if (buffer == NULL) {
            VirtualFree(addresses, 0, MEM_RELEASE);
            return debug_results::write_buffer;
        }

        //make some calls where a buffer *can* be written to, but isn't actually edited because we pass invalid parameters	
        if (GlobalGetAtomName(INVALID_ATOM, (LPTSTR)buffer, 1) != FALSE || GetEnvironmentVariable((LPSTR)get_string(6), (LPSTR)buffer, 4096 * 4096) != FALSE || GetBinaryType((LPSTR)get_string(7), (LPDWORD)buffer) != FALSE
            || HeapQueryInformation(0, (HEAP_INFORMATION_CLASS)69, buffer, 4096, NULL) != FALSE || ReadProcessMemory(INVALID_HANDLE_VALUE, (LPCVOID)0x69696969, buffer, 4096, NULL) != FALSE
            || GetThreadContext(INVALID_HANDLE_VALUE, (LPCONTEXT)buffer) != FALSE || GetWriteWatch(0, &memory::write_buffer, 0, NULL, NULL, (PULONG)buffer) == 0) {
            result = false;
            error = true;
        }

        if (error == FALSE)
        {
            //all calles failed as they're supposed to
            hits = 4096;
            if (GetWriteWatch(0, buffer, 4096, addresses, &hits, &granularity) != 0)
            {
                result = FALSE;
            }
            else
            {
                //should have zero reads here because GlobalGetAtomName doesn't probe the buffer until other checks have succeeded
                //if there's an API hook or debugger in here it'll probably try to probe the buffer, which will be caught here
                result = hits != 0;
            }
        }

        VirtualFree(addresses, 0, MEM_RELEASE);
        VirtualFree(buffer, 0, MEM_RELEASE);

        return result;
    }

    int exceptions::close_handle_exception() {
        //invalid handle
        HANDLE h_invalid = (HANDLE)0xDEADBEEF;

        __try
        {
            CloseHandle(h_invalid);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            //if we get the exception, we return the right code.
            return debug_results::close_handle_exception;
        }

        return debug_results::none;
    }

    int exceptions::single_step_exception() {
        BOOL debugger_present = TRUE;
        __try
        {
            __asm
            {
                pushfd						//save flag register
                or dword ptr[esp], 0x100	//set trap flag in EFlags
                popfd						//restore flag register
                nop							//does nothing
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) { debugger_present = FALSE; }

        //if the exception was raised, return none
        //if a debugger handled the exception (no exception for us to handle), return detection
        return (debugger_present) ? debug_results::single_step : debug_results::none;
    }

    int exceptions::int_3() {
        __try
        {
            _asm
            {
                int 3;	//0xCC / standard software breakpoint
            }
        }
        //exception is handled by our app = debugger did not attempt to intervene
        __except (EXCEPTION_EXECUTE_HANDLER) { return debug_results::none; }

        //if we don't get the exception, we return the right code.
        return debug_results::int_3_cc;
    }

    int exceptions::int_2d() {
        BOOL found = false;
        __try
        {
            _asm
            {
                int 0x2D;	//kernel breakpoint
            }
        }

        __except (EXCEPTION_EXECUTE_HANDLER) { return debug_results::none; }

        __try
        {
            __asm
            {
                xor eax, eax; //clear the eax register
                int  2dh;     //try to get the debugger to bypass the instruction
                inc  eax;     //set the eax register to 1
                mov found, eax;
            }
        }

        __except (EXCEPTION_EXECUTE_HANDLER) { return debug_results::none; }

        //if we don't get the exception, we return the right code.
        return debug_results::int_2;
    }

    int exceptions::prefix_hop() {
        __try
        {
            _asm
            {
                __emit 0xF3;	//0xF3 0x64 is the prefix rep
                __emit 0x64;
                __emit 0xCC;	//this gets skipped over if being debugged (read exceptions::int_3())
            }
        }

        __except (EXCEPTION_EXECUTE_HANDLER) { return debug_results::none; }

        //if we don't get the exception, we return the right code.
        return debug_results::prefix_hop;
    }

    /*int exceptions::debug_string() {
        SetLastError(0);
        OutputDebugStringA(xorstr_("anti-debugging test."));

        return (GetLastError() != 0) ? debug_results::debug_string : debug_results::none;
    }*/

    /*int timing::rdtsc() {
        //integers for time values
        UINT64 time_a, time_b = 0;
        int time_upper_a, time_lower_a = 0;
        int time_upper_b, time_lower_b = 0;

        _asm
        {
            //rdtsc stores result across EDX:EAX
            rdtsc;
            mov time_upper_a, edx;
            mov time_lower_a, eax;

            //junk code -> skip through breakpoint
            xor eax, eax;
            mov eax, 5;
            shr eax, 2;
            sub eax, ebx;
            cmp eax, ecx

                rdtsc;
            mov time_upper_b, edx;
            mov time_lower_b, eax;
        }

        time_a = time_upper_a;
        time_a = (time_a << 32) | time_lower_a;

        time_b = time_upper_b;
        time_b = (time_b << 32) | time_lower_b;

        //0x10000 is purely empirical and is based on the computer's clock cycle, could be less if the cpu clocks really fast etc.
        //should change depending on the length and complexity of the code between each rdtsc operation (-> asm code inbetween needs longer to execute but takes A LOT longer if its being debugged / someone is stepping through it)
        return (time_b - time_a > 0x10000) ? debug_results::rdtsc : debug_results::none;
    }

    int timing::query_performance_counter() {
        LARGE_INTEGER t1;
        LARGE_INTEGER t2;

        QueryPerformanceCounter(&t1);

        //junk code
        _asm
        {
            xor eax, eax;
            push eax;
            push ecx;
            pop eax;
            pop ecx;
            sub ecx, eax;
            shl ecx, 4;
        }

        QueryPerformanceCounter(&t2);

        //30 is a random value
        return ((t2.QuadPart - t1.QuadPart) > 30) ? debug_results::query_performance_counter : debug_results::none;
    }*/

    int timing::get_tick_count() {
        DWORD t1;
        DWORD t2;

        t1 = GetTickCount64();

        //junk code to keep the cpu busy for a few cycles so that time passes and the return value of GetTickCount() changes (so we can detect if it runs at "normal" speed or is being checked through by a human)
        _asm
        {
            xor eax, eax;
            push eax;
            push ecx;
            pop eax;
            pop ecx;
            sub ecx, eax;
            shl ecx, 4;
        }

        t2 = GetTickCount64();

        //30 ms seems ok
        return ((t2 - t1) > 30) ? debug_results::query_performance_counter : debug_results::none;
    }

    int cpu::hardware_debug_registers() {
        CONTEXT ctx = { 0 };
        HANDLE h_thread = GetCurrentThread();

        ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        if (GetThreadContext(h_thread, &ctx))
        {
            return ((ctx.Dr0 != 0x00) || (ctx.Dr1 != 0x00) || (ctx.Dr2 != 0x00) || (ctx.Dr3 != 0x00) || (ctx.Dr6 != 0x00) || (ctx.Dr7 != 0x00)) ? debug_results::hardware_debug_registers : debug_results::none;
        }

        return debug_results::none;
    }

    int cpu::mov_ss() {
        BOOL found = FALSE;

        _asm
        {
            push ss;
            pop ss;
            pushfd;
            test byte ptr[esp + 1], 1;
            jne fnd;
            jmp end;
        fnd:
            mov found, 1;
        end:
            nop;
        }

        return (found) ? debug_results::mov_ss : debug_results::none;
    }

    int virtualization::check_cpuid() {
        bool found = false;
        __asm {
            xor eax, eax
            mov    eax, 0x40000000
            cpuid
            cmp ecx, 0x4D566572
            jne nop_instr
            cmp edx, 0x65726177
            jne nop_instr
            mov found, 0x1
            nop_instr:
            nop
        }

        return (found) ? debug_results::check_cpuid : debug_results::none;
    }

    int virtualization::check_registry() {
        HKEY h_key = 0;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, xorstr_("HARDWARE\\ACPI\\DSDT\\VBOX__"), 0, KEY_READ, &h_key) == ERROR_SUCCESS) { return debug_results::check_registry; }

        return debug_results::none;
    }

    debug_results check_security() {
        if (memory::being_debugged_peb() != debug_results::none) {
            return debug_results::being_debugged_peb;
        }
        if (memory::remote_debugger_present() != debug_results::none) {
            return debug_results::remote_debugger_present;
        }
        if (memory::check_window_name() != debug_results::none) {
            return debug_results::find_window;
        }
        if (memory::is_debugger_present() != debug_results::none) {
            return debug_results::debugger_is_present;
        }
        if (memory::nt_global_flag_peb() != debug_results::none) {
            return debug_results::being_debugged_peb;
        }
        if (memory::nt_query_information_process() != debug_results::none) {
            return debug_results::nt_query_information_process;
        }
        if (memory::debug_active_process() != debug_results::none) {
            return debug_results::debug_active_process;
        }
        if (memory::write_buffer() != debug_results::none) {
            return debug_results::write_buffer;
        }
        if (exceptions::close_handle_exception() != debug_results::none) {
            return debug_results::close_handle_exception;
        }
        if (exceptions::single_step_exception() != debug_results::none) {
            return debug_results::single_step;
        }
        if (exceptions::int_3() != debug_results::none) {
            return debug_results::int_3_cc;
        }
        if (exceptions::int_2d() != debug_results::none) {
            return debug_results::int_2;
        }
        if (exceptions::prefix_hop() != debug_results::none) {
            return debug_results::prefix_hop;
        }
        /*if (exceptions::debug_string() != debug_results::none) {
            return debug_results::debug_string;
        }
        /*if (timing::rdtsc() != debug_results::none) {
            return debug_results::rdtsc;
        }
        if (timing::query_performance_counter() != debug_results::none) {
            return debug_results::query_performance_counter;
        }*/
        if (timing::get_tick_count() != debug_results::none) {
            return debug_results::get_tick_count;
        }
        if (cpu::hardware_debug_registers() != debug_results::none) {
            return debug_results::hardware_debug_registers;
        }
        if (cpu::mov_ss() != debug_results::none) {
            return debug_results::mov_ss;
        }
        if (virtualization::check_cpuid() != debug_results::none) {
            return debug_results::check_cpuid;
        }
        if (virtualization::check_registry() != debug_results::none) {
            return debug_results::check_registry;
        }
        if (check_process() != debug_results::none) {
            return debug_results::debug_active_process;
        }

        return debug_results::none;
    }
}