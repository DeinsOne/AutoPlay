#pragma once

#include "AutoplayCore.h"
#include "AutoplayLog.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/utils/jsoncpp_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>

namespace APlay {

    class ConfigValidator {
        public:
            static const Json::Value* fetchDocumentJson(const std::string &uri) {
                // std::cout << "Fetching " << uri << "..." << std::endl;
                Json::Value* _value = new Json::Value();

                if (!valijson::utils::loadDocument(uri.substr(0, 7) == "file://" ? uri.substr(7, uri.size()) : uri, *_value)) { return nullptr; }

                return _value;
            }

            ConfigValidator(std::string _filename, const Json::Value& _value) : value(_value), filename(_filename) {
                APLAY_PROFILE_FUNCTION();
                _detectVersion();
                Validate();
            }

            void Validate() {
                std::string schemaFile = "assets/.schema/main-v" + version + ".schema.json";
                std::ifstream configFile(schemaFile);

                if (!configFile.is_open() )
                    _processError("Connot open MAIN schema file coresponding to config version " + version);

                Json::Value _schemeDocument;
                configFile >> _schemeDocument;
                configFile.close();


                valijson::adapters::JsonCppAdapter schemeDocumentAdapter(_schemeDocument);
                valijson::adapters::JsonCppAdapter targetDocumentAdapter(value);

                valijson::Schema schema;
                valijson::SchemaParser parser;
                valijson::ValidationResults results;

                try {
                    parser.populateSchema(schemeDocumentAdapter, schema, ConfigValidator::fetchDocumentJson,
                        [](const Json::Value* adapter) { 
                            delete adapter; 
                        }
                    );
                } catch (std::exception &e) {
                    _processError("Failed to parse fetched schema: " + std::string(e.what()));
                }

                valijson::Validator validator(valijson::Validator::kWeakTypes);

                if (!validator.validate(schema, targetDocumentAdapter, &results)) {
                    std::string errMsg = "Failed to validate target document" + filename + "\n";

                    valijson::ValidationResults::Error error;
                    unsigned int errorNum = 1;

                    while (results.popError(error)) {
                        errMsg += "Error #" + std::to_string(errorNum) + '\n';
                        errMsg += "  ";
                        for (const std::string &contextElement : error.context) {
                            errMsg += contextElement + " ";
                        }
                        errMsg += '\n';
                        errMsg += "    - " + error.description + '\n';
                        ++errorNum;
                    }

                    _processError(errMsg);
                }

                _validateTargets();
                _validateRules();

                status = 0;
            }

            int GetStatus() { return status; }

        private:
            void _processError(std::string errorMsg) {
                status = 1;
                throw std::logic_error("Err ConfigValidator in " + filename + " -> " + errorMsg );
            }

            void _validateWithValue(const valijson::Schema& schema, const Json::Value& tovalidate) {
                valijson::adapters::JsonCppAdapter targetDocumentAdapter(tovalidate);

                valijson::ValidationResults results;

                valijson::Validator validator(valijson::Validator::kWeakTypes);

                if (!validator.validate(schema, targetDocumentAdapter, &results)) {
                    std::string errMsg = "Failed to validate properties\n";

                    valijson::ValidationResults::Error error;
                    unsigned int errorNum = 1;

                    while (results.popError(error)) {
                        errMsg += "Error #" + std::to_string(errorNum) + '\n';
                        errMsg += "  ";
                        for (const std::string &contextElement : error.context) {
                            errMsg += contextElement + " ";
                        }
                        errMsg += '\n';
                        errMsg += "    - " + error.description + '\n';
                        ++errorNum;
                    }

                    _processError(errMsg);
                }
            }

            void _validateTargets() {
                std::string schemaFile = "assets/.schema/targets-v" + version + ".schema.json";
                std::ifstream configFile(schemaFile);

                if (!configFile.is_open() )
                    _processError("Connot open TARGET schema file coresponding to config version " + version);

                Json::Value _schemeDocument;
                configFile >> _schemeDocument;
                configFile.close();

                valijson::Schema schema;
                valijson::SchemaParser parser;
                valijson::adapters::JsonCppAdapter schemeDocumentAdapter(_schemeDocument);

                try {
                    parser.populateSchema(schemeDocumentAdapter, schema, ConfigValidator::fetchDocumentJson,
                        [](const Json::Value* adapter) { 
                            delete adapter; 
                        }
                    );
                } catch (std::exception &e) {
                    _processError("Failed to parse fetched schema: " + std::string(e.what()));
                }

                for (auto i : value["targets"].getMemberNames() ) {
                    _validateWithValue(schema, value["targets"][i.c_str()]);
                }
            }

            void _validateRules() {
               std::string schemaFile = "assets/.schema/rules-v" + version + ".schema.json";
                std::ifstream configFile(schemaFile);

                if (!configFile.is_open() )
                    _processError("Connot open RULES schema file coresponding to config version " + version);

                Json::Value _schemeDocument;
                configFile >> _schemeDocument;
                configFile.close();

                valijson::Schema schema;
                valijson::SchemaParser parser;
                valijson::adapters::JsonCppAdapter schemeDocumentAdapter(_schemeDocument);

                try {
                    parser.populateSchema(schemeDocumentAdapter, schema, ConfigValidator::fetchDocumentJson,
                        [](const Json::Value* adapter) { 
                            delete adapter; 
                        }
                    );
                } catch (std::exception &e) {
                    _processError("Failed to parse fetched schema: " + std::string(e.what()));
                }

                for (auto i : value["rules"].getMemberNames() ) {
                    _validateWithValue(schema, value["rules"][i.c_str()]);
                }
            }

            void _detectVersion() {
                if (value["version"].empty() || !value["version"].isString() )
                    _processError("Configuration version field is empty. Can't detect validation schema");
                version = value["version"].asString();
            }

        private:
            std::string version;
            std::string filename;
            Json::Value value;

            int status = -1;
    };


}

