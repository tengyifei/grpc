/*
 *
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Generates Objective C gRPC service interface out of Protobuf IDL.

#include <memory>

#include "src/compiler/c_generator.h"
#include "src/compiler/c_generator_helpers.h"
#include "src/compiler/config.h"
#include "src/compiler/cpp_generator.h"
#include "src/compiler/cpp_generator_helpers.h"

namespace grpc_c_generator {

using std::map;

class ProtoBufCMethod : public grpc_cpp_generator::Method {
 public:
  ProtoBufCMethod(const grpc::protobuf::MethodDescriptor *method)
      : method_(method) {}

  grpc::string name() const { return method_->name(); }

  grpc::string input_type_name() const { return method_->input_type()->name(); }

  grpc::string output_type_name() const {
    return method_->output_type()->name();
  }

  bool NoStreaming() const {
    return !method_->client_streaming() && !method_->server_streaming();
  }

  bool ClientOnlyStreaming() const {
    return method_->client_streaming() && !method_->server_streaming();
  }

  bool ServerOnlyStreaming() const {
    return !method_->client_streaming() && method_->server_streaming();
  }

  bool BidiStreaming() const {
    return method_->client_streaming() && method_->server_streaming();
  }

  grpc::string GetLeadingComments() const {
    return GetCComments(method_, true);
  }

  grpc::string GetTrailingComments() const {
    return GetCComments(method_, false);
  }

 private:
  const grpc::protobuf::MethodDescriptor *method_;
};

class ProtoBufCService : public grpc_cpp_generator::Service {
 public:
  ProtoBufCService(const grpc::protobuf::ServiceDescriptor *service)
      : service_(service) {}

  grpc::string name() const { return service_->name(); }

  int method_count() const { return service_->method_count(); };

  std::unique_ptr<const grpc_cpp_generator::Method> method(int i) const {
    return std::unique_ptr<const grpc_cpp_generator::Method>(
        new ProtoBufCMethod(service_->method(i)));
  };

  grpc::string GetLeadingComments() const {
    return GetCComments(service_, true);
  }

  grpc::string GetTrailingComments() const {
    return GetCComments(service_, false);
  }

 private:
  const grpc::protobuf::ServiceDescriptor *service_;
};

class ProtoBufCPrinter : public grpc_cpp_generator::Printer {
 public:
  ProtoBufCPrinter(grpc::string *str)
      : output_stream_(str), printer_(&output_stream_, '$') {}

  void Print(const std::map<grpc::string, grpc::string> &vars,
             const char *string_template) {
    printer_.Print(vars, string_template);
  }

  void Print(const char *string) { printer_.Print(string); }

  void Indent() { printer_.Indent(); }

  void Outdent() { printer_.Outdent(); }

 private:
  grpc::protobuf::io::StringOutputStream output_stream_;
  grpc::protobuf::io::Printer printer_;
};

class ProtoBufCFile : public CFile {
 public:
  ProtoBufCFile(const grpc::protobuf::FileDescriptor *file) : file_(file) {}

  grpc::string filename() const { return file_->name(); }

  grpc::string filename_without_ext() const {
    return grpc_generator::StripProto(filename());
  }

  // TODO(yifeit): We're relying on Nanopb right now. After rolling out our own
  // Protobuf-C impl, we should
  // use a different extension e.g. ".pbc.h"
  grpc::string message_header_ext() const { return ".pbc.h"; }

  grpc::string service_header_ext() const { return ".grpc.pbc.h"; }

  grpc::string package() const { return file_->package(); }

  std::vector<grpc::string> package_parts() const {
    return grpc_generator::tokenize(package(), ".");
  }

  grpc::string additional_headers() const { return ""; }

  int service_count() const { return file_->service_count(); };

  std::unique_ptr<const grpc_cpp_generator::Service> service(int i) const {
    return std::unique_ptr<const grpc_cpp_generator::Service>(
        new ProtoBufCService(file_->service(i)));
  }

  std::unique_ptr<grpc_cpp_generator::Printer> CreatePrinter(
      grpc::string *str) const {
    return std::unique_ptr<grpc_cpp_generator::Printer>(
        new ProtoBufCPrinter(str));
  }

  grpc::string GetLeadingComments() const { return GetCComments(file_, true); }

  grpc::string GetTrailingComments() const {
    return GetCComments(file_, false);
  }

  virtual std::vector<const google::protobuf::Descriptor *> messages() const {
    std::vector<const google::protobuf::Descriptor *> msgs;
    for (int i = 0; i < file_->message_type_count(); i++) {
      msgs.push_back(file_->message_type(i));
    }
    return msgs;
  }

  virtual std::vector<std::unique_ptr<CFile> > dependencies() const {
    std::vector<std::unique_ptr<CFile> > deps;
    for (int i = 0; i < file_->dependency_count(); i++) {
      std::unique_ptr<CFile> dep(new ProtoBufCFile(file_->dependency(i)));
      // recursively add dependencies
      auto child_dependency = dep->dependencies();
      std::move(child_dependency.begin(), child_dependency.end(),
                std::back_inserter(deps));
      // add myself by moving
      deps.push_back(std::move(dep));
    }
    return deps;
  }

 private:
  const grpc::protobuf::FileDescriptor *file_;
};

}  // namespace grpc_c_generator

class CGrpcGenerator : public grpc::protobuf::compiler::CodeGenerator {
 public:
  CGrpcGenerator() {}
  virtual ~CGrpcGenerator() {}

  virtual bool Generate(const grpc::protobuf::FileDescriptor *file,
                        const ::grpc::string &parameter,
                        grpc::protobuf::compiler::GeneratorContext *context,
                        ::grpc::string *error) const {
    grpc::string file_name = grpc_generator::StripProto(file->name());

    // TODO(yifeit): Add c_prefix option in protobuf, and update descriptor
    //::grpc::string prefix = file->options().c_prefix();
    grpc::string prefix = "";

    if (file->options().cc_generic_services()) {
      *error =
          "C grpc proto compiler plugin does not work with generic "
          "services. To generate cpp grpc APIs, please set \""
          "cc_generic_service = false\".";
      return false;
    }

    grpc_c_generator::Parameters generator_parameters;
    generator_parameters.use_system_headers = true;

    grpc_c_generator::ProtoBufCFile pbfile(file);

    if (!parameter.empty()) {
      std::vector<grpc::string> parameters_list =
          grpc_generator::tokenize(parameter, ",");
      for (auto parameter_string = parameters_list.begin();
           parameter_string != parameters_list.end(); parameter_string++) {
        std::vector<grpc::string> param =
            grpc_generator::tokenize(*parameter_string, "=");
        if (param[0] == "grpc_search_path") {
          generator_parameters.grpc_search_path = param[1];
        } else if (param[0] == "nanopb_headers_prefix") {
          generator_parameters.nanopb_headers_prefix = param[1];
        } else if (param[0] == "use_system_headers") {
          if (param[1] == "true") {
            generator_parameters.use_system_headers = true;
          } else if (param[1] == "false") {
            generator_parameters.use_system_headers = false;
          } else {
            *error = grpc::string("Invalid parameter: ") + *parameter_string;
            return false;
          }
        } else {
          *error = grpc::string("Unknown parameter: ") + *parameter_string;
          return false;
        }
      }
    }

    grpc::string header_code =
        grpc_c_generator::GetHeaderPrologue(&pbfile, generator_parameters) +
        grpc_c_generator::GetHeaderIncludes(&pbfile, generator_parameters) +
        grpc_c_generator::GetHeaderServices(&pbfile, generator_parameters) +
        grpc_c_generator::GetHeaderEpilogue(&pbfile, generator_parameters);
    std::unique_ptr<grpc::protobuf::io::ZeroCopyOutputStream> header_output(
        context->Open(file_name + ".grpc.pbc.h"));
    grpc::protobuf::io::CodedOutputStream header_coded_out(header_output.get());
    header_coded_out.WriteRaw(header_code.data(), header_code.size());

    grpc::string source_code =
        grpc_c_generator::GetSourcePrologue(&pbfile, generator_parameters) +
        grpc_c_generator::GetSourceIncludes(&pbfile, generator_parameters) +
        grpc_c_generator::GetSourceServices(&pbfile, generator_parameters) +
        grpc_c_generator::GetSourceEpilogue(&pbfile, generator_parameters);
    std::unique_ptr<grpc::protobuf::io::ZeroCopyOutputStream> source_output(
        context->Open(file_name + ".grpc.pbc.c"));
    grpc::protobuf::io::CodedOutputStream source_coded_out(source_output.get());
    source_coded_out.WriteRaw(source_code.data(), source_code.size());

    return true;
  }

 private:
  // Insert the given code into the given file at the given insertion point.
  void Insert(grpc::protobuf::compiler::GeneratorContext *context,
              const grpc::string &filename, const grpc::string &insertion_point,
              const grpc::string &code) const {
    std::unique_ptr<grpc::protobuf::io::ZeroCopyOutputStream> output(
        context->OpenForInsert(filename, insertion_point));
    grpc::protobuf::io::CodedOutputStream coded_out(output.get());
    coded_out.WriteRaw(code.data(), code.size());
  }
};

int main(int argc, char *argv[]) {
  CGrpcGenerator generator;
  return grpc::protobuf::compiler::PluginMain(argc, argv, &generator);
}
