// tbs_native.cpp
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//
// Minimal nanobind wrapper around the Windows TPM Base Services (TBS) API.
// It replaces the previous ctypes-based access to the TBS functions
// Tbsi_Context_Create, Tbsip_Submit_Command and Tbsip_Context_Close.

#include <nanobind/nanobind.h>

#include <windows.h>
#include <tbs.h>

#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

namespace nb = nanobind;

namespace {

// Maximum size of a TPM response buffer (matches the previous ctypes code).
constexpr UINT32 kMaxResponseSize = 4096;

std::string FormatError(const char* function, TBS_RESULT result) {
    char buffer[128];
    std::snprintf(buffer, sizeof(buffer), "%s() failed: error 0x%08X", function,
                  static_cast<unsigned int>(result));
    return std::string(buffer);
}

// RAII wrapper around a TBS context handle. The handle is created on
// construction and released either explicitly via close() or on destruction.
class TbsContext {
public:
    TbsContext() {
        TBS_CONTEXT_PARAMS2 params{};
        params.version = TBS_CONTEXT_VERSION_TWO;
        params.includeTpm20 = 1;

        TBS_RESULT result = Tbsi_Context_Create(
            reinterpret_cast<PCTBS_CONTEXT_PARAMS>(&params), &handle_);
        if (result != TBS_SUCCESS) {
            throw std::runtime_error(FormatError("Tbsi_Context_Create", result));
        }
    }

    ~TbsContext() {
        if (handle_ != nullptr) {
            // Best effort cleanup; never throw from the destructor.
            Tbsip_Context_Close(handle_);
            handle_ = nullptr;
        }
    }

    TbsContext(const TbsContext&) = delete;
    TbsContext& operator=(const TbsContext&) = delete;

    nb::bytes SubmitCommand(nb::bytes command) {
        if (handle_ == nullptr) {
            throw std::runtime_error("TBS context is not open");
        }

        std::vector<BYTE> response(kMaxResponseSize);
        UINT32 responseLength = static_cast<UINT32>(response.size());

        TBS_RESULT result = Tbsip_Submit_Command(
            handle_,
            TBS_COMMAND_LOCALITY_ZERO,
            TBS_COMMAND_PRIORITY_NORMAL,
            static_cast<const BYTE*>(command.data()),
            static_cast<UINT32>(command.size()),
            response.data(),
            &responseLength);
        if (result != TBS_SUCCESS) {
            throw std::runtime_error(FormatError("Tbsip_Submit_Command", result));
        }

        return nb::bytes(reinterpret_cast<const char*>(response.data()),
                         responseLength);
    }

    void Close() {
        if (handle_ != nullptr) {
            TBS_RESULT result = Tbsip_Context_Close(handle_);
            handle_ = nullptr;
            if (result != TBS_SUCCESS) {
                throw std::runtime_error(
                    FormatError("Tbsip_Context_Close", result));
            }
        }
    }

private:
    TBS_HCONTEXT handle_ = nullptr;
};

}  // namespace

NB_MODULE(tbs_native, m) {
    m.doc() = "Minimal nanobind wrapper around the Windows TPM Base Services "
              "(TBS) API.";

    nb::class_<TbsContext>(m, "TbsContext")
        .def(nb::init<>(), "Create a TBS context for a TPM 2.0 device.")
        .def("submit_command", &TbsContext::SubmitCommand, nb::arg("command"),
             "Submit a TPM command buffer and return the response buffer.")
        .def("close", &TbsContext::Close,
             "Close the TBS context and release the associated resources.");
}
