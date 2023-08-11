#include "handler/meme_handler.h"
#include <string>

MemeHandler::MemeHandler(std::string location, std::string root, std::shared_ptr<MemeModule> memes) 
  :location_(location), root_(root), memes_(memes)
{
    template_to_url_["Two-Buttons"] = "https://imgflip.com/s" + location_ + "/Two-Buttons.jpg";
    template_to_url_["One-Does-Not-Simply"] = "https://imgflip.com/s" + location_ + "/One-Does-Not-Simply.jpg";
    template_to_url_["Buff-Doge-vs-Cheems"] = "https://imgflip.com/s"+ location_ + "/Buff-Doge-vs-Cheems.png";
    template_to_url_["Drake-Hotline-Bling"] = "https://imgflip.com/s" + location_ + "/Drake-Hotline-Bling.jpg";
    template_to_url_["Left-Exit-12-Off-Ramp"] = "https://imgflip.com/s" + location_ + "/Left-Exit-12-Off-Ramp.jpg";
    template_to_url_["Mocking-Spongebob"] = "https://imgflip.com/s" + location_ + "/Mocking-Spongebob.jpg";
    template_to_url_["Change-My-Mind"] = "https://imgflip.com/s" + location_ + "/Change-My-Mind.jpg";
}

bool MemeHandler::check_format(const std::string& target, const std::string& url)
{
    std::size_t lastSlashPosition = target.find_last_of("/");
    std::string possible_id = target.substr(lastSlashPosition + 1);
    try {
        std::stoi(possible_id);
        return target.substr(0, lastSlashPosition + 1) == url;
    } catch (const std::invalid_argument& ia) {
        return false;
    }
}

bool MemeHandler::check_format_floating(const std::string& target, const std::string& url)
{
    std::size_t lastSlashPosition = target.find_last_of("/");
    std::string possible_float = target.substr(lastSlashPosition + 1);
    try {
        std::stof(possible_float);
        return target.substr(0, lastSlashPosition + 1) == url;
    } catch (const std::invalid_argument& ia) {
        return false;
    }
}

status MemeHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    Logger *logger = Logger::getLogger();
    logger->log_info("Starting Meme Handler");

    http::verb method = req.method();

    if (method == http::verb::post) {
        logger->log_info("Handling POST request");
        return handle_post(req, res);
    } else if (method == http::verb::get) {
        logger->log_info("Handling GET request");
        return handle_get(req, res);
    }

    
    return http::status::not_found;
}


/**
 * @brief Handle a POST request to location_
 *
 * @param req The request to handle
 * @param res The response to fill out
 * @return status The status of the response
 */
status MemeHandler::handle_post(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    // std::lock_guard<std::mutex> lock(memes_->meme_mutex_);
    if (req.target().find(location_) == std::string::npos) {
        return http::status::not_found;
    }
    Logger *logger = Logger::getLogger();
    logger->log_info("Handling POST request to " + location_);

    if (req.target() ==location_ + "/comment") {
        return handle_post_comment(req, res);
    } else if (req.target() == location_) {
        return handle_post_meme(req, res);
    } else {
        logger->log_response_data(404,req.target().to_string(),"MemeHandler");
        return http::status::not_found;
    }
}

status MemeHandler::handle_post_meme(const http::request<http::string_body> &req, http::response<http::string_body> &res)
{
    Logger *logger = Logger::getLogger();
    logger->log_info("Creating meme");
    unsigned int lowest_id = 1;
    Meme meme = Meme(req.body());
    if (!memes_->create(lowest_id, meme)) {
        logger->log_error("Error creating meme");
        logger->log_response_data(500,req.target().to_string(),"MemeHandler");
        return http::status::internal_server_error;
    } else {
        logger->log_info("Meme created with id " + std::to_string(lowest_id));
    }
    logger->log_response_data(201,req.target().to_string(),"MemeHandler");
    status res_status = http::status::created;
    int res_version = 11;
    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, "text/plain");

    // set response body
    res.body() = "Successfully created meme " + std::to_string(lowest_id) + ".";

    // prepare response
    res.prepare_payload();

    return res_status;
}

status MemeHandler::handle_post_comment(const http::request<http::string_body> &req, http::response<http::string_body> &res)
{
    Logger *logger = Logger::getLogger();
    logger->log_info("Adding comment to meme");
    Comment comment = Comment(req.body());
    int id;
    int rating;
    try {
        id = std::stoi(comment.id_);
        rating = std::stoi(comment.rating_);        
    } catch (const std::invalid_argument& ia) {
        logger->log_response_data(404,req.target().to_string(),"MemeHandler");
        return http::status::not_found;
    }
    memes_->add_rating(id, rating);
    memes_->add_comment(id, comment);
    logger->log_response_data(201,req.target().to_string(),"MemeHandler");
    status res_status = http::status::created;
    int res_version = 11;
    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, "text/plain");

    // set response body
    res.body() = "Successfully posted comment.";

    // prepare response
    res.prepare_payload();

    return res_status;
}

/**
 * @brief Handle a GET request to location_
 *
 * @param req The request to handle
 * @param res The response to fill out
 * @return status The status of the response
 */
status MemeHandler::handle_get(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    if (req.target() == location_ + "/creation_form") {
        return handle_creation_form(req, res);
    } else if (req.target() == location_ + "/random") {
        return handle_get_random_meme(req, res);
    } else if (req.target() == location_ + "/all") {
        return handle_get_all_memes(req, res);
    } else if (check_format(req.target().to_string(), location_ + "/")) {
        return handle_get_meme_by_id(req, res);
    } else if (check_format(req.target().to_string(), location_ + "/info/")) {
        return handle_get_other_info_by_id(req, res);
    } else if (req.target() == location_ + "/comment_form") {
        return handle_post_comment_form(req, res);
    } else if (check_format(req.target().to_string(), location_ + "/get_by_views/")) {
        return handle_get_meme_by_views(req, res);
    } else if (check_format_floating(req.target().to_string(), location_ + "/get_by_rating/")) {
        return handle_get_meme_by_rating(req, res);
    } else {
        return http::status::not_found;
    }
}

status MemeHandler::handle_creation_form(const http::request<http::string_body>& req, http::response<http::string_body>& res){
    if (req.target() != location_ + "/creation_form") {
        return http::status::not_found;
    }
    Logger *logger = Logger::getLogger();
    logger->log_info("Generating new form");


    // set response header
    status res_status = http::status::ok;
    int res_version = 11;
    std::string res_content_type = "text/html";

    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, res_content_type);

    // set form to be hard-coded HTML

    form_ = "<html> <style>";
    form_ += "input[type=text], select {";
    form_ += "width: 100%;";
    form_ +=  "padding: 12px 20px;";
    form_ += "margin: 8px 0;";
    form_ += "display: inline-block;";
    form_ += "border: 1px solid #ccc;";
    form_ += "border-radius: 4px;";
    form_ += "box-sizing: border-box;";
    form_ +=  "}";

    form_ += "input[type=submit] {";
    form_ += "width: 100%;";
    form_ += "background-color: #4CAF50;";
    form_ += "color: white;";
    form_ +=  "padding: 14px 20px;";
    form_ += "margin: 8px 0;";
    form_ += "border: none;";
    form_ += "border-radius: 4px;";
    form_ += "cursor: pointer;";
    form_ +=  "}";

    form_ += "input[type=submit]:hover {";
    form_ +=  "background-color: #45a049;";
    form_ += "}";

    form_ += "div {";
    form_ += "border-radius: 5px;";
    form_ += "background-color: #f2f2f2;";
    form_ += "padding: 20px;";
    form_ += "}";
    form_ += "</style>";

    form_ += "<body> <div> <form id = \"new_meme\" method = \"\" action=\"" + location_ + "/creation_form\" onsubmit = \"runCode(event)\">";
    form_ += "<label for=\"top\">Top text</label>";
    form_ +="<input type=\"text\" id=\"top\" name=\"top\" placeholder=\"Top text..\">";

    form_ +="<label for=\"bottom\">Bottom text</label>";
    form_ +="<input type=\"text\" id=\"bottom\" name=\"bottom\" placeholder=\"Bottom text..\">";

    form_ +="<label for=\"image\">Image</label>";
    form_ +="<select id=\"img\" name=\"img\">";
    form_ +="<option value = \"select\">Select an image template </option>";
    form_ +="<option value=\"One-Does-Not-Simply\">one does not simply</option>";
    form_ +="<option value=\"Two-Buttons\">two buttons</option>";
    form_ +="<option value=\"Buff-Doge-vs-Cheems\">buff doge vs Cheems</option>";
    form_ +="<option value=\"Drake-Hotline-Bling\">drake hotline bling</option>";
    form_ +="<option value=\"Left-Exit-12-Off-Ramp\">left exit 12 off ramp</option>";
    form_ +="<option value=\"Mocking-Spongebob\">mocking spongebob</option>";
    form_ +="<option value=\"Change-My-Mind\">change my mind</option>";

    form_ +="</select>";
  
    form_ += "<button type=\"submit\"> Submit</button>" ;
    form_ += "</form> </div>";
    std::string end = "";
    form_ = form_ 
    + "<script>"
    + "function runCode(event) {"
    +  "event.preventDefault(); "

    +  "var top = document.getElementById(\"top\").value;"
    +  "var bottom = document.getElementById(\"bottom\").value;"
    +  "var template = document.querySelector(\'select[name=\"img\"]\').value;"
    +  "var data = {"
    +    "top: top,"
    +    "bottom:bottom,"
    +    "template:template"
    +  "};"
    +  "var xhr = new XMLHttpRequest();"
    +  "xhr.open(\"POST\", \"" + location_ + "\", true);"
    +  "xhr.onreadystatechange = function() {"
    +  "console.log(xhr.responseText);"
    +  "};"

    +  "xhr.setRequestHeader(\"Content-Type\", \"application/json\");"
    +  "xhr.send(JSON.stringify(data))"
    + "}"
    + ("</script> </body> </html>"
    + end);

    // set response body
    res.body() = form_;
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");

    // prepare response
    res.prepare_payload();
    return res_status;
}

status MemeHandler::handle_post_comment_form(const http::request<http::string_body>& req, http::response<http::string_body>& res){

    if (req.target() != location_ + "/comment_form") {
        return http::status::not_found;
    }
    Logger *logger = Logger::getLogger();
    logger->log_info("Generating new comment form");

    status res_status = http::status::ok;
    int res_version = 11;
    std::string res_content_type = "text/html";

    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, res_content_type);

    comment_form_ = "<html> <style>";
    comment_form_ += "input[type=text], select {";
    comment_form_ += "width: 100%;";
    comment_form_ +=  "padding: 12px 20px;";
    comment_form_ += "margin: 8px 0;";
    comment_form_ += "display: inline-block;";
    comment_form_ += "border: 1px solid #ccc;";
    comment_form_ += "border-radius: 4px;";
    comment_form_ += "box-sizing: border-box;";
    comment_form_ +=  "}";

    comment_form_ += "button {";
    comment_form_ += "width: 100%;";
    comment_form_ += "background-color: #4CAF50;";
    comment_form_ += "color: white;";
    comment_form_ += "padding: 14px 20px;";
    comment_form_ += "margin: 8px 0;";
    comment_form_ += "border: none;";
    comment_form_ += "border-radius: 4px;";
    comment_form_ += "cursor: pointer;";
    comment_form_ += "}";

    comment_form_ += "button {";
    comment_form_ +=  "background-color: #45a049;"; // green
    comment_form_ += "}";

    comment_form_ += "div {";
    comment_form_ += "border-radius: 5px;";
    comment_form_ += "background-color: #ffffff;";
    comment_form_ += "padding: 20px;";
    comment_form_ += "}";
    comment_form_ += "</style>";

    comment_form_ += "<body> <div> <form id = \"meme_comment\" method = \"post\" action=\"" + location_ + "/comment_form\" onsubmit = \"runCode(event)\">";
    comment_form_ += "<label for=\"Comment\">Comment</label>";
    comment_form_ += "<input type=\"text\" id=\"Comment\" name=\"Comment\" placeholder=\"Your comment..\">";

    comment_form_ += "<label for=\"MemeID\">Meme ID</label>";
    comment_form_ += "<input type=\"text\" id=\"MemeID\" name=\"MemeID\" >";
    comment_form_ += "<label for=\"rating\">Rating</label>";
    comment_form_ += "<select id=\"rating\" name=\"rating\">";
    comment_form_ += "<option value = \"1\">1 </option>";
    comment_form_ += "<option value = \"2\">2</option>";
    comment_form_ += "<option value = \"3\">3</option>";
    comment_form_ += "<option value = \"4\">4</option>";
    comment_form_ += "<option value = \"5\">5</option>";
    comment_form_ += "</select>";

    comment_form_ += "<button type=\"submit\"> Submit</button>" "</form> </div>";
    
    std::string end = "";
    comment_form_ = comment_form_ 
    + "<script>"
    + "function runCode(event) {"
    +  "event.preventDefault(); "

    +  "var comment = document.getElementById(\"Comment\").value;"
    +  "var id = document.getElementById(\"MemeID\").value;"
    +  "var rating = document.querySelector(\'select[name=\"rating\"]\').value;"

    +  "var data = {"
    +  "comment: comment,"
    +  "id: id," 
    +  "rating:rating"
    +  "};"
    +  "var xhr = new XMLHttpRequest();"
    +  "xhr.open(\"POST\", \"" + location_ + "/comment\", true);"
    +  "xhr.setRequestHeader(\"Content-Type\", \"application/json\");"
    +  "xhr.send(JSON.stringify(data))"
    + "}"
    + ("</script> </body> </html>"
    + end);

    // set response body
    res.body() = comment_form_;
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");

    // prepare response
    res.prepare_payload();
    return res_status;
}

/**
 * @brief Handle a POST request to location_
 *
 * @param req The request to handle
 * @param res The response to fill out
 * @return status The status of the response
 */
status MemeHandler::handle_get_random_meme(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    Logger *logger = Logger::getLogger();
    int count = memes_->get_meme_count();
    logger->log_info("Getting random meme from " + std::to_string(count) + " memes");
    if (count == 0) {
        logger->log_error("No memes to get");
        logger->log_response_data(400,req.target().to_string(),"MemeHandler");
        return http::status::bad_request;
    }
    int random_id = rand() % count + 1;
    logger->log_info("Random id: " + std::to_string(random_id));
    std::shared_ptr<Meme> meme = memes_->get(random_id);
    if (meme == nullptr) {
        logger->log_error("Error getting meme");
        logger->log_response_data(500,req.target().to_string(),"MemeHandler");
        return http::status::internal_server_error;
    }
    res.result(http::status::ok);
    res.version(11);
    res.body() = meme->content_;
    res.set(http::field::content_type, "text/html");
    res.prepare_payload();
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");
    return http::status::ok;
}

status MemeHandler::handle_get_meme_by_id(const http::request<http::string_body> &req, http::response<http::string_body> &res)
{
    std::size_t lastSlashPosition = req.target().to_string().find_last_of("/");
    std::string possible_id = req.target().to_string().substr(lastSlashPosition + 1);
    int id = stoi(possible_id);
    Logger *logger = Logger::getLogger();
    logger->log_info("Getting meme with id " + std::to_string(id));
    std::shared_ptr<Meme> meme = memes_->get(id);
    if (meme == nullptr) {
        logger->log_error("Error getting meme");
        logger->log_response_data(500,req.target().to_string(),"MemeHandler");
        return http::status::internal_server_error;
    }

    std::string url = template_to_url_[meme->template_];

    logger->log_info("Meme template: " + meme->template_);
    logger->log_info("Meme top text: " + meme->top_);
    logger->log_info("Meme bottom text: " + meme->bottom_);
    std::string display_html_content =
        "<html>\n"
        "<style>\n"
        "body { display: inline-block; position: relative; }\n"
        "span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }\n"
        "#top { top: 0; color: blue;}\n"
        "#bottom { bottom: 0; color: blue;}\n"
        "#subtitle {"
        "bottom: -100px; "
        "color: red; /* Choose your desired color */"
        "font-size: 1.0em; "
        "}"
        "</style>\n"
        "<body>\n"
        "<img src=\"" + url + "\">\n"
        "<br/>\n"
        "<span id=\"top\">" + meme->top_ + "</span>\n""<span id=\"bottom\">" + meme->bottom_ + "</span>\n"
        "<a id=\"subtitle\" href = \"" + location_+ "/info/" + std::to_string(id)+ "\">" + "Viewed by: "+ std::to_string(meme->view_count_)+". Click for more information< /a>\n"
        "<span\n>\n"
        "</body>\n"
        "</html>\n";
    res.result(http::status::ok);
    res.version(11);
    res.body() = display_html_content;
    res.set(http::field::content_type, "text/html");
    res.prepare_payload();
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");

    return http::status::ok;
}

status MemeHandler::handle_get_all_memes(const http::request<http::string_body> &req, http::response<http::string_body> &res)
{
    int count = memes_->get_meme_count();
    std::string list = "<html> <style>";
    list += "a:link {";
    list += "color: blue;";
    list += "}";

    list += "a:hover {";
    list += "text-decoration: underline;";
    list += "} </style>";
    list += "<body> <h2>List of memes</h2>";
    for(int i = 1; i <= count; i++){
        //std::shared_ptr<Meme> meme = memes_->get(i);
            list += "Number of views: " + std::to_string(memes_->get_view_count(i)) + "   Link to meme: <a href=\"";
            list += location_ + "/" + std::to_string(i) + "\">";
            list += std::to_string(i);
            list += "</a><br>";
        
    }    
    
    list += "<a href=\"";
    list += location_ + "/comment_form\">Post a comment";
    list += "</body> </html>";
    Logger *logger = Logger::getLogger();    
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");
    res.result(http::status::ok);
    res.version(11);
    res.body() = list;
    res.set(http::field::content_type, "text/html");
    res.prepare_payload();
    return http::status::ok;
}

status MemeHandler::handle_get_meme_by_views(const http::request<http::string_body> &req, http::response<http::string_body> &res)
{
    std::size_t lastSlashPosition = req.target().to_string().find_last_of("/");
    std::string possible_views = req.target().to_string().substr(lastSlashPosition + 1);
    int views = stoi(possible_views);
    int count = memes_->get_meme_count();
    std::string list = "<html> <style>";
    list += "a:link {";
    list += "color: blue;";
    list += "}";

    list += "a:hover {";
    list += "text-decoration: underline;";
    list += "} </style>";
    list += "<body> <h2>List of memes</h2>";
    for(int i = 1; i <= count; i++){
        //std::shared_ptr<Meme> meme = memes_->get(i);
        if(memes_->get_view_count(i) > views){
            list += "Number of views: " + std::to_string(memes_->get_view_count(i)) + "   Link to meme: <a href=\"";
            list += location_ + "/" + std::to_string(i) + "\">";
            list += std::to_string(i);
            list += "</a><br>";
        }
    }    
    
    list += "<a href=\"";
    list += location_ + "/comment_form\">Post a comment";
    list += "</body> </html>";
    Logger *logger = Logger::getLogger();    
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");
    res.result(http::status::ok);
    res.version(11);
    res.body() = list;
    res.set(http::field::content_type, "text/html");
    res.prepare_payload();
    return http::status::ok;
}

status MemeHandler::handle_get_meme_by_rating(const http::request<http::string_body> &req, http::response<http::string_body> &res)
{
    std::size_t lastSlashPosition = req.target().to_string().find_last_of("/");
    std::string possible_float = req.target().to_string().substr(lastSlashPosition + 1);
    float rating = stof(possible_float);

    int count = memes_->get_meme_count();
    std::string list = "<html> <style>";
    list += "a:link {";
    list += "color: blue;";
    list += "}";

    list += "a:hover {";
    list += "text-decoration: underline;";
    list += "} </style>";
    list += "<body> <h2>List of memes</h2>";
    for(int i = 1; i <= count; i++){
        if(memes_->get_rating(i) > rating){
            list += "Number of views: " + std::to_string(memes_->get_view_count(i)) + "   Link to meme: <a href=\"";
            list += location_ + "/" + std::to_string(i) + "\">";
            list += std::to_string(i);
            list += "</a><br>";
        }
    }    
    
    list += "<a href=\"";
    list += location_ + "/comment_form\">Post a comment";
    list += "</body> </html>";
    Logger *logger = Logger::getLogger();    
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");
    res.result(http::status::ok);
    res.version(11);
    res.body() = list;
    res.set(http::field::content_type, "text/html");
    res.prepare_payload();
    return http::status::ok;
}


status MemeHandler::handle_get_other_info_by_id(const http::request<http::string_body> &req, http::response<http::string_body> &res)
{
    std::size_t lastSlashPosition = req.target().to_string().find_last_of("/");
    std::string possible_id = req.target().to_string().substr(lastSlashPosition + 1);
    int id = stoi(possible_id);
    Logger *logger = Logger::getLogger();
    logger->log_info("Getting info for meme with id " + std::to_string(id));
    std::vector<Comment> comments = memes_->get_comments(id);
//    if (comments.size() == 0) {
//        logger->log_error("No comments found");
//        logger->log_response_data(404,req.target().to_string(),"MemeHandler");
//        return http::status::bad_request;
//    }
    int views = memes_->get_view_count(id);
    float ratings = memes_->get_rating(id);
    boost::property_tree::ptree comments_tree;
    for (Comment comment : comments)
    {
        boost::property_tree::ptree child;
        child.put_value(comment.comment_);
        comments_tree.push_back(std::make_pair("", child));
    }

    //comments_tree.push_back(boost::property_tree::ptree::value_type("", comment));

    //only returning views and ratings for now since comments is not implemented
    boost::property_tree::ptree pt;
    pt.put("views", views);
    pt.put("ratings", ratings);
    pt.add_child("comments", comments_tree);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    res.result(http::status::ok);
    res.version(11);
    res.body() = ss.str();
    res.set(http::field::content_type, "application/json");
    res.prepare_payload();
    logger->log_response_data(200,req.target().to_string(),"MemeHandler");
    return http::status::ok;
}