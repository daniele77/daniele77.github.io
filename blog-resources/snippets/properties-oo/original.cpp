#include <iostream>
#include <vector>
#include <functional>
#include <regex>

using namespace std;

using Json = std::string;

class Storage {
public:
    virtual unsigned selectMaxIssueID() const = 0;
    virtual Json insertIssueIncreasedID(const Json &requestedIssue) = 0;
    virtual std::vector<Json> allIssues() const = 0;
    virtual Json issue(unsigned id) const = 0;
};

struct Response
{
    Json content;
    int httpCode;

    static Response ok(Json response) { return{ response, 200 }; }
    static Response badRequest() { return status(400); }
    static Response notFound() { return status(404); }
    static Response methodNotAllowed() { return status(405); }
private:
    static Response status(int st) { return{ Json{}, st }; }
};

class RestApi
{
public:
    Response process(std::string const& requestUri, std::string const& requestMethod, std::string const& requestContent) const 
    {

        using ResourceFunction = std::function<Response(std::string const&, std::smatch const&)>;
        struct Resource
        {
            std::string uriPattern;
            std::string allowedMethod;
            ResourceFunction impl;
        };

        std::vector<Resource> resources{
            {
                "/issue/new",
                "POST",
                [this](std::string const& requestContent, std::smatch const& id_match) {
                    return issue_new(requestContent);
                }
            },
            {
                "/issue/list",
                "GET",
                [this](std::string const& requestContent, std::smatch const& id_match) {
                    return issue_list();
                }
            },
            {
                "/issue/([0-9]*)",
                "GET",
                [this](std::string const& requestContent, std::smatch const& id_match) {
                    auto id_string = id_match[1].str();
                    return issue_id(id_string);
                }
            }
        };

        for (auto const& resource : resources)
        {
            std::regex uriRegex{ resource.uriPattern };
            std::smatch uriMatch;
            if (std::regex_match(requestUri, uriMatch, uriRegex)) {
                if (requestMethod != resource.allowedMethod) {
                    return Response::methodNotAllowed();
                }
                return resource.impl(requestContent, uriMatch);
            }
        }

        return Response::notFound();
    }

private:

    Response issue_id(std::string const& id_string) const
    {
        Json issue = storage.issue(stoul(id_string));
        if (issue.empty()) return Response::notFound();
        return Response::ok(issue);
    }

    Response issue_list() const 
    {
        Json data{ { "issues", Json::array() } };
        auto all_issues = storage.allIssues();
        for (Json issue : all_issues) {
            issue["data"].erase("description");
            data["issues"].push_back(std::move(issue["data"]));
        }
        return Response::ok(Json{ { "data", data } });
    }

    Response issue_new(std::string const& requestContent) const {
        try {
            auto issueJson = Json::parse(requestContent);
            auto parsedIssue = IssueData::parse(issueJson);
            if (!parsedIssue.success) {
                return Response::badRequest();
            }

            auto requestedIssue = parsedIssue.issueData;
            return{ storage.insertIssueIncreasedID(requestedIssue.toStorageJson()), 200 };
        }
        catch (std::invalid_argument&) {
            return Response::badRequest();
        }
    }

    Storage& storage;
};

int main()
{
    cout << "Arne code\n";
    return 0;
}