#pragma once
#include <iostream>
#include <comdef.h>
#include <WbemIdl.h>

#pragma comment(lib, "wbemuuid.lib")

class CWMISearcher {
public:
	explicit CWMISearcher(std::wstring namespaceName)
        : namespace_name_(std::move(namespaceName)) {
        // Initialize COM library
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to initialize COM library.");
        }

        // Initialize security settings
        hr = CoInitializeSecurity(
            nullptr,
            -1,
            nullptr,
            nullptr,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            nullptr,
            EOAC_NONE,
            nullptr
        );
        if (FAILED(hr)) {
            CoUninitialize();
            throw std::runtime_error("Failed to initialize COM security settings.");
        }

        // Initialize WMI
        hr = CoCreateInstance(
            CLSID_WbemLocator,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            reinterpret_cast<LPVOID*>(&locator_)
        );
        if (FAILED(hr)) {
            CoUninitialize();
            throw std::runtime_error("Failed to create WbemLocator instance.");
        }

        // Connect to the specified namespace
        hr = locator_->ConnectServer(
            _bstr_t(namespace_name_.c_str()),
            nullptr,
            nullptr,
            nullptr,
            NULL,
            nullptr,
            nullptr,
            &services_
        );
        if (FAILED(hr)) {
            Cleanup();
            throw std::runtime_error("Failed to connect to the specified namespace.");
        }

        // Set security levels on the proxy
        hr = CoSetProxyBlanket(
            services_,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            nullptr,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            nullptr,
            EOAC_NONE
        );
        if (FAILED(hr)) {
            Cleanup();
            throw std::runtime_error("Failed to set proxy blanket.");
        }
    }

    ~CWMISearcher() {
        Cleanup();
    }

    // Execute a WMI query and return the results as a string
    std::wstring ExecuteQuery(const std::wstring& query, const std::wstring& prop_name_of_result_object) const
    {
        IEnumWbemClassObject* enumerator = nullptr;

        HRESULT hr = services_->ExecQuery(
            bstr_t("WQL"),
            bstr_t(query.c_str()),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr,
            &enumerator
        );

        if (FAILED(hr)) {
            throw std::runtime_error("Failed to execute WMI query.");
        }

        // Process the query results
        IWbemClassObject* object = nullptr;
        ULONG returned_count = 0;
        std::wstring result;

        while (enumerator) {
            hr = enumerator->Next(WBEM_INFINITE, 1, &object, &returned_count);

            // No more items in the result set
            if (hr == WBEM_S_FALSE) {
                break; 
            }

            if (FAILED(hr)) {
                throw std::runtime_error("Failed to retrieve query results.");
            }

            // Process the query result here, for example, extract properties
           
            VARIANT var;
            hr = object->Get(prop_name_of_result_object.c_str(), 0, &var, nullptr, nullptr);
            if (SUCCEEDED(hr) && var.vt == VT_BSTR) {
                result += _bstr_t(var.bstrVal);
                result += L"\n";
            }

            object->Release();
        }

        if (enumerator) {
            enumerator->Release();
        }

        return result;
    }

private:
    void Cleanup() {
        if (services_) {
            services_->Release();
            services_ = nullptr;
        }
        if (locator_) {
            locator_->Release();
            locator_ = nullptr;
        }
        CoUninitialize();
    }

    std::wstring namespace_name_;
    IWbemLocator* locator_ = nullptr;
    IWbemServices* services_ = nullptr;
};
