#include <iostream>
#include <vector>
#include <functional>
#include <regex>
#include <memory>

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

class Request
{
public:
    Request(const std::string& _uriPattern, const std::string& _method) :
        uriPattern(_uriPattern), method(_method)
    {}
    virtual ~Request() = default;
    virtual Response Handle(const std::string& requestContent) = 0;
private:
    const std::string uriPattern;
    const std::string method;
};

class NewIssueRequest : public Request
{
public:
    NewIssueRequest() : Request("/issue/new", "POST") {}
    virtual Response Handle(const std::string& requestContent) override
    {
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
};

class IssueListRequest : public Request
{
public:
    IssueListRequest() : Request("/issue/list", "GET") {}
    virtual Response Handle(const std::string& requestContent) override
    {
        Json data{ { "issues", Json::array() } };
        auto all_issues = storage.allIssues();
        for (Json issue : all_issues) {
            issue["data"].erase("description");
            data["issues"].push_back(std::move(issue["data"]));
        }
        return Response::ok(Json{ { "data", data } });
    }
};

class IssueDetailRequest : public Request
{
public:
    IssueDetailRequest() : Request("/issue/list", "GET") {}
    virtual Response Handle(const std::string& requestContent) override
    {
        auto id_string = id_match[1].str();
        return issue_id(id_string);
    }
private:
    Response issue_id(std::string const& id_string) const
    {
        Json issue = storage.issue(stoul(id_string));
        if (issue.empty()) return Response::notFound();
        return Response::ok(issue);
    }
};

class RestApi
{
public:
    Response process(std::string const& requestUri, std::string const& requestMethod, std::string const& requestContent) const
    {
        for (auto& request: requests)
        {
            request->Handle(requestContent);
            /*
            std::regex uriRegex{ resource.uriPattern };
            std::smatch uriMatch;
            if (std::regex_match(requestUri, uriMatch, uriRegex)) {
                if (requestMethod != resource.allowedMethod) {
                    return Response::methodNotAllowed();
                }
                return resource.impl(requestContent, uriMatch);
            }
            */
        }

        return Response::notFound();
    }

private:

    std::vector<std::unique_ptr<Request> > requests;
    Storage& storage;
};


int main()
{
	cout << "OO code\n";
	return 0;
}