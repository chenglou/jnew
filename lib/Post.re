
let (|?) = (x, y) => switch x { | None => y | Some(x) => x };
let (|?>) = (x, f) => switch x { | None => None | Some(x) => f(x) };
let (|?>>) = (x, f) => switch x { | None => None | Some(x) => Some(f(x)) };
let (|!) = (x, y) => switch x { | None => failwith(y) | Some(x) => x };

let spacer = Shared.spacer;
let showDate = Shared.showDate;

let postAbout = (~css, ~date, ~tags, ~withPic=true, ~children, ()) => {
  open Html;
  open Css;
  <div className=css([
    A("color", Shared.Colors.lightText),
    A("font-family", "Open sans"),
    A("font-size", "14px"),
    A("display", "flex"),
    A("flex-direction", "row"),
    A("align-items", "center"),
    A("justify-content", "flex-start"),
  ])>
    <showDate date />
    (spacer(8))
    " · "
    (spacer(8))
    (String.concat(", " ++ spacer(4), List.map(tag => <a
      href=("/tags/" ++ tag ++ "/")
      className=css([A("text-decoration", "none")])
    >tag</a>, tags)))
  </div>
};

let pageWithTopAndBottom = Shared.pageWithTopAndBottom;
let pageHead = Shared.pageHead;

let render = (posts, ({Types.title: contentTitle, fileName, description, date, tags, thumbnail}, _, rawBody)) => {
  let (css, inlineCss) = Css.startPage();
  open Html;
  open Css;
  let isMarkdown = Filename.check_suffix(fileName, ".md");

  let main = AboutMe.bodyWithSmallAboutMeColumn;
  let body = <main css toc=(
    <div className=css([A("padding", "0 16px"), A("flex-shrink", "1"), A("overflow", "auto")])>
      <div className=css([])>
        "Recent posts"
      </div>
      (Shared.hspace(8))
      (List.mapi(
        (i, ({Types.title, date: (year, month, day), fileName}, _, _)) => {
          let href = ("/" ++ Filename.chop_extension(fileName) ++ "/");
          i < 5 ? <a
            href
            className=css([
            A("font-size", "16px"),
            A("margin-bottom", "16px"),
            A("display", "block"),
            A("line-height", "20px"),
            ...Shared.Styles.subtleLink
          ])>
            <div>
            title
            </div>
            <div className=css([
              A("color", Shared.Colors.lightText),
              ...Shared.Styles.row
            ])>
            (string_of_int(year))
            (Shared.monthName(month))
            (string_of_int(day))
            </div>
          </a> : ""
        },
        posts
      ) |> String.concat("\n"))
      (Shared.hspace(32))
    </div>
  )>

    <h1 className=css(Shared.Styles.titleWithTopMargin)>contentTitle</h1>
    <postAbout css date tags />
    (Shared.hspace(32))
    <div className=css(Shared.Styles.bodyText)>
      (isMarkdown ? MarkdownParser.parse(rawBody) : rawBody)
    </div>
  </main>;

  <html>
    <pageHead
      title=contentTitle
      ?description
      ?thumbnail
    >
      <style>(inlineCss())</style>
    </pageHead>

    body
  </html>
};

let postList = (posts, tags, contentTitle) => {
  open Html;
  let (css, inlineCss) = Css.startPage();
  /* let contentTitle = "All posts"; */
  let main = AboutMe.bodyWithSmallAboutMeColumn;

  let body = <main css toc=(
    <div className=css([
      A("font-size", "16px"),
      A("line-height", "20px"),
      A("margin-bottom", "32px"),
    ])>
      (List.map(
        ((tag, count)) => {
          open Types;
            <a href=("/tags/" ++ tag ++ "/") className=css([
                A("color", "currentColor"),
                A("white-space", "nowrap"),
                A("margin-right", "8px"),
                ...Shared.Styles.hoverUnderline
              ])
            >
              tag ("" ++ string_of_int(count) ++ "")
            </a>
            /* <postAbout css date=config.date tags=config.tags withPic=false />
            <a className=css([A("font-size", "24px")]) href>readTime</a> */
        },
        tags
      ) |> String.concat("\n"))
    </div>
  )>
    <div className=css([A("flex", "3"), A("padding", "32px")])>
      <div className=css([A("padding", "1px"), A("position", "relative")])>
        <h1 className=css([
          A("text-align", "center"),
          ...Shared.Styles.title
        ])>contentTitle</h1>
      </div>
      (List.map(
        ((config, teaser, _)) => {
          open Types;
          let href = ("/" ++ Filename.chop_extension(config.Types.fileName) ++ "/");
          let readTime = Shared.minuteReadText(config.wordCount);
          <div>
            <a href className=css([
                A("color", "currentColor"),
                A("text-decoration", "none")
              ])
            >
              <h2>(config.title)</h2>
            </a>
            <postAbout css date=config.date tags=config.tags withPic=false />
            (switch teaser {
            | None => ""
            | Some(teaser) =>
              <div className=css([A("padding-top", "16px"), ...Shared.Styles.bodyText])>
                (Omd.to_html(Omd.of_string(teaser)))
              </div>
            })
            <a className=css([A("font-size", "24px")]) href>readTime</a>
          </div>
        },
        posts
      ) |> String.concat("\n<div style='height: 32px'></div>\n"))
      </div>
  </main>;

  <html>
    <pageHead
      title=contentTitle
      description="Things Jared has written about"
    >
      <style>(inlineCss())</style>
    </pageHead>
    body
  </html>
};


open Types;
let defaultConfig = fileName => {
  fileName,
  title: "JaredForsyth.com",
  tags: [],
  categories: [],
  date: (0, 0, 0), /* Year, Month, Day */
  description: None,
  thumbnail: None,
  featured: false,
  wordCount: None
};

let check = (opt, base, fn) => switch opt {
| None => base
| Some(value) => fn(value)
};

let parseDate = text => {
  Scanf.sscanf(text, "%d-%d-%d", (year, month, day) => (year, month, day))
};

let parseConfig = (fileName, opts) => {
  let config = defaultConfig(fileName);
  let config = check(Toml.string("title", opts), config, title => {...config, title});
  let config = check(Toml.string("description", opts), config, description => {...config, description: Some(description)});
  let config = check(Toml.stringList("tags", opts), config, tags => {...config, tags});
  let config = check(Toml.stringList("categories", opts), config, categories => {...config, categories});
  let config = check(Toml.string("date", opts), config, date => {...config, date: parseDate(date)});
  let config = check(Toml.bool("featured", opts), config, featured => {...config, featured});
  config
};

let getIntro = body => switch (Str.split(Str.regexp("<!-- more -->"), body)) {
| [] => assert(false)
| [one] => None
| [top, ...rest] => Some(top)
};

let parse = (fileName, opts, content) => {
  let opts = opts |! "No options for post " ++ fileName;
  let config = parseConfig(fileName, opts);
  let intro = getIntro(content);
  let isMarkdown = Filename.check_suffix(fileName, ".md");
  let wordCount = isMarkdown ? Some(Str.split(Str.regexp("[^a-zA-Z0-9-]+"), content) |> List.length) : None;
  let config = {...config, wordCount};

  (config, intro, content)
};