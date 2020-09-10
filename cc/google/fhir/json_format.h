/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GOOGLE_FHIR_JSON_FORMAT_H_
#define GOOGLE_FHIR_JSON_FORMAT_H_

#include <string>

#include "google/protobuf/message.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "absl/time/time.h"
#include "google/fhir/annotations.h"
#include "google/fhir/primitive_handler.h"
#include "google/fhir/r4/profiles.h"
#include "google/fhir/status/status.h"
#include "google/fhir/stu3/profiles.h"

namespace google {
namespace fhir {

class Parser {
 public:
  explicit Parser(const PrimitiveHandler* primitive_handler)
      : primitive_handler_(primitive_handler) {}

  // Merges a string of raw FHIR json into an existing message.
  // Takes a default timezone for timelike data that does not specify timezone.
  // For reading JSON into a new resource, it is recommended to use
  // JsonFhirStringToProto or JsonFhirStringToProtoWithoutValidating.
  ::absl::Status MergeJsonFhirStringIntoProto(const std::string& raw_json,
                                              google::protobuf::Message* target,
                                              absl::TimeZone default_timezone,
                                              const bool validate) const;

  // Given a template for a FHIR resource type, creates a resource proto of that
  // type and merges a std::string of raw FHIR json into it.
  // Returns a status error if the JSON string was not a valid resource
  // according to the requirements of the requested FHIR proto. Takes a default
  // timezone for timelike data that does not specify timezone.
  template <typename R>
  ::absl::StatusOr<R> JsonFhirStringToProto(
      const std::string& raw_json,
      const absl::TimeZone default_timezone) const {
    R resource;
    FHIR_RETURN_IF_ERROR(MergeJsonFhirStringIntoProto(raw_json, &resource,
                                                      default_timezone, true));
    return resource;
  }

  // Given a template for a FHIR resource type, creates a resource proto of that
  // type and merges a std::string of raw FHIR json into it.
  // Will not validate FHIR requirements such as required fields, but will fail
  // if it encounters a field it cannot convert.
  // Takes a default timezone for timelike data that does not specify timezone.
  template <typename R>
  ::absl::StatusOr<R> JsonFhirStringToProtoWithoutValidating(
      const std::string& raw_json,
      const absl::TimeZone default_timezone) const {
    R resource;
    FHIR_RETURN_IF_ERROR(MergeJsonFhirStringIntoProto(raw_json, &resource,
                                                      default_timezone, false));
    return resource;
  }

 private:
  const PrimitiveHandler* primitive_handler_;
};

class Printer {
 public:
  explicit Printer(const PrimitiveHandler* primitive_handler)
      : primitive_handler_(primitive_handler) {}

  // Prints a FHIR primitive to string for display.  This string conforms to the
  // FHIR regex for this primitive.
  ::absl::StatusOr<std::string> PrintFhirPrimitive(
      const ::google::protobuf::Message& primitive_message) const;

  // Prints a FHIR proto to a single line of FHIR JSON, suitable for NDJSON
  ::absl::StatusOr<std::string> PrintFhirToJsonString(
      const google::protobuf::Message& fhir_proto) const;

  // Prints a FHIR proto to "pretty" (i.e., multi-line) FHIR JSON.
  ::absl::StatusOr<std::string> PrettyPrintFhirToJsonString(
      const google::protobuf::Message& fhir_proto) const;

  // Prints a FHIR proto to a single line of FHIR Analytic JSON,
  // suitable for NDJSON
  ::absl::StatusOr<std::string> PrintFhirToJsonStringForAnalytics(
      const google::protobuf::Message& fhir_proto) const;

  // Prints a FHIR proto to "pretty" (i.e., multi-line) FHIR Analytic JSON.
  ::absl::StatusOr<std::string> PrettyPrintFhirToJsonStringForAnalytics(
      const google::protobuf::Message& fhir_proto) const;

 private:
  const PrimitiveHandler* primitive_handler_;
};

}  // namespace fhir
}  // namespace google

#endif  // GOOGLE_FHIR_JSON_FORMAT_H_
