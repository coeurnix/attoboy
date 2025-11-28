# Builds a single-page HTML app for the attoboy documentation in all languages.
# Usage: python docs2html.py --output html/index.html
# Requires the Python `markdown` package listed in requirements.txt.

import argparse
import html
import json
import re
from pathlib import Path

import markdown

LANG_NAMES = {"en": "English", "ru": "Russian", "zh": "Chinese"}
DEFAULT_LANGS = ("en", "ru", "zh")

def slugify(value: str) -> str:
  value = value.lower()
  value = re.sub(r"[^\w]+", "-", value)
  return value.strip("-") or "item"

HTML_TEMPLATE = """
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>attoboy Documentation</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
:root {
  --bg: #070b16;
  --panel: #0c1224;
  --card: #111a30;
  --accent: #7ce0d6;
  --accent-2: #7ca6ff;
  --muted: #b6c2e1;
  --border: #1d2944;
  --shadow: 0 20px 80px rgba(6, 10, 20, 0.6);
}
* { box-sizing: border-box; }
body { margin: 0; background: radial-gradient(circle at 20% 20%, rgba(124,160,255,0.08), transparent 35%), radial-gradient(circle at 80% 0%, rgba(124,224,214,0.08), transparent 35%), var(--bg); color: #eef2ff; font-family: "Space Grotesk","Segoe UI",sans-serif; line-height: 1.6; }
a { color: var(--accent); text-decoration: none; }
a:hover { color: var(--accent-2); }
.muted { color: var(--muted); }
.topbar { position: sticky; top: 0; z-index: 10; backdrop-filter: blur(12px); background: rgba(7,11,22,0.9); border-bottom: 1px solid var(--border); display: flex; align-items: center; justify-content: space-between; padding: 14px 18px; gap: 16px; }
.brand { display: flex; align-items: center; gap: 12px; }
.brand-icon { width: 44px; height: 44px; border-radius: 12px; background: linear-gradient(135deg, #7ce0d6, #7ca6ff); display: grid; place-items: center; color: #0b1022; font-weight: 700; letter-spacing: 1px; text-decoration: none; }
.brand-text { display: flex; flex-direction: column; gap: 2px; }
.brand-title { font-size: 18px; font-weight: 700; }
.brand-sub { font-size: 12px; color: var(--muted); }
.top-actions { display: flex; align-items: center; gap: 12px; flex: 1; justify-content: flex-end; }
.github-link { padding: 10px 14px; border-radius: 10px; border: 1px solid var(--border); color: #eef2ff; background: #0f162b; box-shadow: var(--shadow); transition: transform 0.15s ease, border-color 0.15s ease; }
.github-link:hover { transform: translateY(-2px); border-color: var(--accent); }
.lang-switch { display: flex; gap: 8px; }
.lang-switch button { border: 1px solid var(--border); background: #0f162b; color: #eef2ff; border-radius: 10px; padding: 9px 12px; cursor: pointer; transition: all 0.18s ease; }
.lang-switch button.active { border-color: var(--accent); background: rgba(124,224,214,0.12); color: var(--accent); }
.lang-switch button:hover { border-color: var(--accent-2); color: var(--accent-2); }
.search-box { position: relative; width: 340px; max-width: 100%; }
.search-box input { width: 100%; padding: 10px 12px; border-radius: 12px; border: 1px solid var(--border); background: #0f162b; color: #eef2ff; outline: none; box-shadow: inset 0 1px 0 rgba(255,255,255,0.03); }
.search-box input:focus { border-color: var(--accent); }
.search-results { position: absolute; top: calc(100% + 8px); left: 0; right: 0; background: #0f162b; border: 1px solid var(--border); border-radius: 12px; box-shadow: var(--shadow); max-height: 360px; overflow-y: auto; display: none; }
.search-results.open { display: block; }
.search-results .result { padding: 10px 12px; border-bottom: 1px solid rgba(255,255,255,0.04); cursor: pointer; }
.search-results .result:last-child { border-bottom: none; }
.search-results .title { font-weight: 600; margin-bottom: 4px; }
.search-results .snippet { color: var(--muted); font-size: 13px; line-height: 1.4; }
.search-results mark { background: rgba(124,224,214,0.2); color: #fff; padding: 0 2px; border-radius: 4px; }
.layout { display: grid; grid-template-columns: 320px 1fr; gap: 22px; padding: 20px 18px 40px; }
.sidebar { background: rgba(15,22,43,0.7); border: 1px solid var(--border); border-radius: 16px; padding: 16px; box-shadow: var(--shadow); position: sticky; top: 92px; height: calc(100vh - 120px); overflow-y: auto; }
.nav-group-label { text-transform: uppercase; letter-spacing: 0.04em; font-size: 12px; color: var(--muted); margin-top: 12px; margin-bottom: 6px; }
.nav-item { width: 100%; text-align: left; border: 1px solid transparent; background: transparent; color: #eef2ff; padding: 10px 10px; border-radius: 10px; cursor: pointer; transition: all 0.18s ease; }
.nav-item:hover { background: rgba(255,255,255,0.04); border-color: var(--border); }
.nav-item.active { background: linear-gradient(135deg, rgba(124,224,214,0.16), rgba(124,166,255,0.16)); border-color: var(--accent); color: #fff; }
.nav-sub { margin: 6px 0 10px 8px; border-left: 1px solid var(--border); padding-left: 8px; display: grid; gap: 6px; }
.nav-sub-item { width: 100%; text-align: left; border: 1px solid transparent; background: rgba(255,255,255,0.02); color: var(--muted); padding: 6px 8px; border-radius: 8px; cursor: pointer; transition: all 0.18s ease; font-size: 13px; }
.nav-sub-item:hover { border-color: var(--border); color: #eef2ff; }
.nav-sub-item.active { border-color: var(--accent); color: #fff; background: rgba(124,224,214,0.08); }
.content-card { background: rgba(15,22,43,0.9); border: 1px solid var(--border); border-radius: 16px; padding: 24px 24px 32px; box-shadow: var(--shadow); }
article h1, article h2, article h3, article h4 { color: #fff; }
article h1 { font-size: 32px; margin-bottom: 14px; }
article h2 { font-size: 24px; margin-top: 32px; }
article h3 { font-size: 20px; margin-top: 22px; }
article p { color: #d9e0f7; }
article code { background: rgba(255,255,255,0.05); color: #e8f3ff; padding: 2px 6px; border-radius: 6px; font-size: 0.95em; }
article pre { background: #0b1022; border: 1px solid var(--border); border-radius: 12px; padding: 14px; overflow: auto; }
article pre code { display: block; background: transparent; padding: 0; color: #e8f3ff; }
article table { border-collapse: collapse; width: 100%; margin: 16px 0; }
article th, article td { border: 1px solid var(--border); padding: 8px; }
article blockquote { border-left: 3px solid var(--accent); margin: 18px 0; padding-left: 12px; color: var(--muted); }
.tok-keyword { color: #7ca6ff; }
.tok-string { color: #8ee6b2; }
.tok-number { color: #ffd580; }
.tok-comment { color: #8291b4; }
.tok-directive { color: #f0a8ff; }
@media (max-width: 980px) {
  .layout { grid-template-columns: 1fr; }
  .sidebar { position: static; height: auto; }
}
</style>
</head>
<body>
<div class="topbar">
  <div class="brand">
    <a class="brand-icon" id="brand-link" href="#">A</a>
    <div class="brand-text">
      <div class="brand-title">attoboy</div>
      <div class="brand-sub">C++ library documentation</div>
    </div>
  </div>
  <div class="top-actions">
    <a class="github-link" href="https://github.com/coeurnix/attoboy" target="_blank" rel="noopener">GitHub</a>
    <div class="lang-switch" id="language-switch"></div>
    <div class="search-box">
      <input id="search-input" type="search" placeholder="Search the docs...">
      <div id="search-results" class="search-results"></div>
    </div>
  </div>
</div>
<div class="layout">
  <aside class="sidebar">
    <div class="nav-group-label">Navigate</div>
    <div id="nav-list"></div>
  </aside>
  <main class="content">
    <div class="content-card">
      <article id="doc-content"></article>
    </div>
  </main>
</div>
<script>
const DOCS = __DOCS__;
const LANGUAGE_NAMES = __LANG_MAP__;
let currentLang = "__DEFAULT_LANG__";
let currentSlug = "home";
let currentAnchor = null;
let suppressHashChange = false;

const navList = document.getElementById("nav-list");
const contentEl = document.getElementById("doc-content");
const searchInput = document.getElementById("search-input");
const searchResults = document.getElementById("search-results");
const langSwitch = document.getElementById("language-switch");
const brandLink = document.getElementById("brand-link");

const FUNCTION_INDEX = {};
Object.keys(DOCS).forEach((lang) => {
  const list = [];
  (DOCS[lang] || []).forEach((page) => {
    (page.functions || []).forEach((fn) => {
      list.push({
        ...fn,
        pageSlug: page.slug,
        pageLabel: page.label,
        search: (fn.signature + " " + fn.synopsis + " " + fn.title).toLowerCase(),
      });
    });
  });
  FUNCTION_INDEX[lang] = list;
});

function getPage(lang, slug) {
  const pages = DOCS[lang] || [];
  for (const page of pages) {
    if (page.slug === slug) return page;
  }
  return null;
}

function ensureSlug(slug, lang) {
  if (getPage(lang, slug)) return slug;
  if (getPage(lang, "home")) return "home";
  const pages = DOCS[lang] || [];
  return pages.length ? pages[0].slug : slug;
}

function navigateTo(slug, anchor = null, langOverride = null) {
  if (langOverride && DOCS[langOverride]) {
    currentLang = langOverride;
  }
  currentSlug = ensureSlug(slug, currentLang);
  currentAnchor = anchor || null;
  renderLanguages();
  renderNavigation();
  renderPage();
  closeSearch();
}

function setLanguage(lang) {
  if (!DOCS[lang]) return;
  currentLang = lang;
  currentSlug = ensureSlug(currentSlug, currentLang);
  renderLanguages();
  renderNavigation();
  renderPage();
  closeSearch();
}

function renderLanguages() {
  langSwitch.innerHTML = Object.keys(DOCS)
    .map((lang) => {
      const active = lang === currentLang ? "active" : "";
      const label = LANGUAGE_NAMES[lang] || lang.toUpperCase();
      return `<button data-lang="${lang}" class="${active}">${label}</button>`;
    })
    .join("");
  langSwitch.querySelectorAll("button").forEach((btn) => {
    btn.addEventListener("click", () => setLanguage(btn.dataset.lang));
  });
}

function renderNavigation() {
  const pages = DOCS[currentLang] || [];
  const baseOrder = ["project-readme", "home", "getting-started", "tutorial", "api-index"];
  const apiPages = pages.filter((p) => p.slug.startsWith("api-") && p.slug !== "api-index");
  const markup = [];
  for (const slug of baseOrder) {
    const page = getPage(currentLang, slug);
    if (page) {
      const active = slug === currentSlug ? "active" : "";
      markup.push(`<button class="nav-item ${active}" data-slug="${slug}">${page.label}</button>`);
    }
  }
  if (apiPages.length) {
    markup.push('<div class="nav-group-label">API Classes</div>');
    for (const page of apiPages) {
      const active = page.slug === currentSlug ? "active" : "";
      markup.push(`<button class="nav-item ${active}" data-slug="${page.slug}">${page.label}</button>`);
      if (page.slug === currentSlug && page.functions && page.functions.length) {
        const sub = page.functions
          .map((fn) => {
            const activeFn = currentAnchor === fn.id ? "active" : "";
            return `<button class="nav-sub-item ${activeFn}" data-slug="${page.slug}" data-anchor="${fn.id}">${fn.title}</button>`;
          })
          .join("");
        markup.push(`<div class="nav-sub">${sub}</div>`);
      }
    }
  }
  navList.innerHTML = markup.join("");
  navList.querySelectorAll(".nav-item").forEach((btn) => {
    btn.addEventListener("click", () => navigateTo(btn.dataset.slug));
  });
  navList.querySelectorAll(".nav-sub-item").forEach((btn) => {
    btn.addEventListener("click", () => navigateTo(btn.dataset.slug, btn.dataset.anchor));
  });
}

function escapeHtml(value) {
  return value.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
}

function highlightCpp(code) {
const patterns = [
    { type: "comment", regex: /\/\*[\s\S]*?\*\/|\/\/.*$/gm },
    { type: "string", regex: /"(?:[^"\\\\]]|\\\\.)*"|'(?:[^'\\\\]]|\\\\.)*'/g },
    { type: "number", regex: /\b0x[0-9a-fA-F]+\b|\b\d+\.\d+f?\b|\b\d+f?\b|\b\d+\b/g },
    { type: "keyword", regex: /\b(?:alignas|alignof|asm|auto|bool|break|case|catch|char|class|const|constexpr|continue|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|nullptr|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while)\b/g },
    { type: "directive", regex: /#[ \t]*[a-zA-Z_]\w*/g },
  ];
  const matches = [];

  for (const pattern of patterns) {
    pattern.regex.lastIndex = 0;
    let m;
    while ((m = pattern.regex.exec(code)) !== null) {
      matches.push({ start: m.index, end: m.index + m[0].length, type: pattern.type });
    }
  }
  matches.sort((a, b) => a.start - b.start || b.end - a.end);
  const filtered = [];
  let lastEnd = -1;
  for (const token of matches) {
    if (token.start < lastEnd) continue;
    filtered.push(token);
    lastEnd = token.end;
  }
  let result = "";
  let cursor = 0;
  for (const token of filtered) {
    if (token.start > cursor) result += escapeHtml(code.slice(cursor, token.start));
    const segment = escapeHtml(code.slice(token.start, token.end));
    result += `<span class="tok-${token.type}">${segment}</span>`;
    cursor = token.end;
  }
  if (cursor < code.length) result += escapeHtml(code.slice(cursor));
  return result;
}

function highlightContent() {
  const blocks = contentEl.querySelectorAll("pre code");
  blocks.forEach((codeEl) => {
    codeEl.innerHTML = highlightCpp(codeEl.textContent);
  });
  const inlines = contentEl.querySelectorAll("article code:not(pre code)");
  inlines.forEach((codeEl) => {
    codeEl.innerHTML = highlightCpp(codeEl.textContent);
  });
}

function renderPage() {
  const page = getPage(currentLang, currentSlug);
  if (!page) return;
  contentEl.innerHTML = page.html;
  document.title = page.title + " | attoboy";
  highlightContent();
  setHashFromState();
  if (currentAnchor) {
    const target = document.getElementById(currentAnchor);
    if (target) {
      target.scrollIntoView({ behavior: "smooth", block: "start" });
    } else {
      window.scrollTo({ top: 0, behavior: "smooth" });
    }
  } else {
    window.scrollTo({ top: 0, behavior: "smooth" });
  }
}

function closeSearch() {
  searchResults.classList.remove("open");
  searchResults.innerHTML = "";
}

function escapeRegex(value) {
  const specials = /[-\/\^$*+?.()|[\]{}]/g;
  return value.replace(specials, "\$&");
}

function highlightTerm(text, term) {
  if (!term) return escapeHtml(text);
  const safe = escapeHtml(text);
  const re = new RegExp("(" + escapeRegex(term) + ")", "ig");
  return safe.replace(re, "<mark>$1</mark>");
}

function snippet(text, query) {
  const lower = text.toLowerCase();
  const term = query.toLowerCase();
  const idx = lower.indexOf(term);
  const span = 80;
  if (idx === -1) return text.slice(0, span * 2);
  const start = Math.max(0, idx - span);
  const end = Math.min(text.length, idx + term.length + span);
  let segment = text.slice(start, end);
  if (start > 0) segment = "..." + segment;
  if (end < text.length) segment = segment + "...";
  const re = new RegExp("(" + escapeRegex(term) + ")", "ig");
  return segment.replace(re, "<mark>$1</mark>");
}

function runSearch(term) {
  const query = term.trim();
  if (!query) {
    closeSearch();
    return;
  }
  const results = [];
  const funcs = FUNCTION_INDEX[currentLang] || [];
  const qLower = query.toLowerCase();
  for (const fn of funcs) {
    if (fn.search.includes(qLower)) {
      results.push({ type: "function", item: fn });
    }
    if (results.length === 5) break;
  }
  if (results.length < 5) {
    const pages = DOCS[currentLang] || [];
    for (const page of pages) {
      if (page.text.toLowerCase().includes(qLower)) {
        results.push({ type: "page", item: page });
      }
      if (results.length === 5) break;
    }
  }
  if (!results.length) {
    searchResults.innerHTML = '<div class="result"><div class="snippet">No matches found</div></div>';
    searchResults.classList.add("open");
    return;
  }
  searchResults.innerHTML = results
    .map((entry) => {
      if (entry.type === "function") {
        const fn = entry.item;
        const sig = highlightTerm(fn.signature, query);
        const syn = fn.synopsis ? highlightTerm(fn.synopsis, query) : "";
        return `<div class="result" data-slug="${fn.pageSlug}" data-anchor="${fn.id}">
        <div class="title">${sig}</div>
        <div class="snippet"><span class="muted">${fn.pageLabel}</span>${syn ? " → " + syn : ""}</div>
      </div>`;
      }
      const page = entry.item;
      const preview = snippet(page.text, query);
      return `<div class="result" data-slug="${page.slug}">
      <div class="title">${page.label}</div>
      <div class="snippet">${preview}</div>
    </div>`;
    })
    .join("");
  searchResults.classList.add("open");
  searchResults.querySelectorAll(".result").forEach((item) => {
    item.addEventListener("click", () => {
      navigateTo(item.dataset.slug, item.dataset.anchor || null);
      closeSearch();
    });
  });
}

function parseHashString(hashValue) {
  if (!hashValue) return null;
  const cleaned = hashValue.replace(/^#\/?/, "");
  if (!cleaned) return null;
  const parts = cleaned.split("/").filter(Boolean);
  if (parts.length >= 2) {
    return { lang: parts[0], slug: parts[1], anchor: parts[2] || null };
  }
  return null;
}

function setHashFromState() {
  const target = `#/${currentLang}/${currentSlug}` + (currentAnchor ? `/${currentAnchor}` : "");
  if (window.location.hash === target) return;
  suppressHashChange = true;
  window.location.hash = target;
  setTimeout(() => {
    suppressHashChange = false;
  }, 0);
}

function applyHash(hashValue) {
  const parsed = parseHashString(hashValue);
  if (!parsed || !DOCS[parsed.lang]) return false;
  currentLang = parsed.lang;
  currentSlug = ensureSlug(parsed.slug, currentLang);
  currentAnchor = parsed.anchor || null;
  renderLanguages();
  renderNavigation();
  renderPage();
  return true;
}

searchInput.addEventListener("input", (event) => runSearch(event.target.value));
searchInput.addEventListener("focus", (event) => runSearch(event.target.value));
searchInput.addEventListener("keydown", (event) => {
  if (event.key === "Escape") closeSearch();
});

contentEl.addEventListener("click", (event) => {
  const link = event.target.closest("a");
  if (!link) return;
  const href = link.getAttribute("href") || "";
  if (href.startsWith("#/")) {
    event.preventDefault();
    const parsed = parseHashString(href);
    if (parsed) {
      navigateTo(parsed.slug, parsed.anchor || null, parsed.lang || currentLang);
    }
  } else if (href.startsWith("#")) {
    event.preventDefault();
    currentAnchor = href.slice(1);
    renderPage();
  }
});

brandLink.addEventListener("click", (event) => {
  event.preventDefault();
  const targetSlug = getPage(currentLang, "project-readme") ? "project-readme" : "home";
  navigateTo(targetSlug);
});

window.addEventListener("hashchange", () => {
  if (suppressHashChange) return;
  applyHash(window.location.hash);
});

const initial = parseHashString(window.location.hash);
if (initial && DOCS[initial.lang]) {
  currentLang = initial.lang;
  currentSlug = ensureSlug(initial.slug, currentLang);
  currentAnchor = initial.anchor || null;
} else {
  currentSlug = ensureSlug(currentSlug, currentLang);
}

renderLanguages();
renderNavigation();
renderPage();
</script>

</body>
</html>
"""


def read_markdown(path: Path) -> str:
  return path.read_text(encoding="utf-8")


def convert_markdown(md_text: str) -> str:
  md = markdown.Markdown(
      extensions=["fenced_code", "tables", "toc", "sane_lists", "codehilite", "attr_list"] 
  )
  return md.convert(md_text)


def extract_title(md_text: str, fallback: str) -> str:
  match = re.search(r"^\s*#\s+(.+)", md_text, re.MULTILINE)
  if match:
    return match.group(1).strip()
  return fallback


def to_plain_text(html_text: str) -> str:
  text = re.sub(r"<[^>]+>", " ", html_text)
  text = html.unescape(text)
  text = re.sub(r"\s+", " ", text)
  return text.strip()


def find_signature(block: str) -> str:
  match = re.search(r"\*{1,2}Signature\*{1,2}.*?```[^\n]*\n(.*?)```", block, re.S)
  if not match:
    match = re.search(r"```[^\n]*\n(.*?)```", block, re.S)
  if match:
    return match.group(1).strip()
  return ""


def find_synopsis(block: str) -> str:
  match = re.search(
      r"\*{1,2}Synopsis\*{1,2}\s*\n+(.+?)(?=(\n\s*\n\*\*|\n####|\n###|\n##|\n#|\Z))",
      block,
      re.S,
  )
  if match:
    synopsis = re.sub(r"\s+", " ", match.group(1)).strip()
    return synopsis
  return ""


def strip_signature_modifiers(signature: str) -> str:
  leading = re.compile(
      r"^(?:(?:inline|static|constexpr|consteval|constinit|virtual|friend|extern|volatile)\s+)+",
      re.I,
  )
  cleaned = leading.sub("", signature).strip()
  cleaned = re.sub(r"\s+", " ", cleaned)
  cleaned = re.sub(
      r"\)\s+(?:const|noexcept(?:\([^)]+\))?|override|final)(?=\s|;|$)",
      ")",
      cleaned,
      flags=re.I,
  )
  cleaned = re.sub(
      r"\s+(?:const|noexcept(?:\([^)]+\))?|override|final)(?=;|$)",
      "",
      cleaned,
      flags=re.I,
  )
  return cleaned.strip()


def extract_functions(md_text: str, page_slug: str) -> tuple[str, list[dict[str, str]]]:
  pattern = re.compile(
      r"^(?P<header>(?P<level>###|####)\s+`?(?P<title>[^`\n]+)`?\s*(?P<attrs>\{#[^}]+\})?\s*\n)(?P<body>.*?)(?=^(?:###|####)\s+`?|^##\s+|^#\s+|\Z)",
      re.M | re.S,
  )
  parts: list[str] = []
  functions: list[dict[str, str]] = []
  last_end = 0
  seen: set[str] = set()
  for match in pattern.finditer(md_text):
    parts.append(md_text[last_end:match.start()])
    title = match.group("title").strip()
    attrs = match.group("attrs")
    level = match.group("level")
    anchor = None
    if attrs:
      anchor_match = re.search(r"\{#([^}]+)\}", attrs)
      if anchor_match:
        anchor = anchor_match.group(1)
    if not anchor:
      base_anchor = slugify(title)
      anchor = base_anchor
      counter = 2
      while anchor in seen:
        anchor = f"{base_anchor}-{counter}"
        counter += 1
      header = f"{level} `{title}` {{#{anchor}}}\n"
    else:
      header = match.group("header")
    seen.add(anchor)
    body = match.group("body")
    signature = strip_signature_modifiers(find_signature(body) or title)
    synopsis = find_synopsis(body)
    functions.append(
        {
            "id": anchor,
            "title": title,
            "signature": signature,
            "synopsis": synopsis,
        }
    )
    parts.append(header + body)
    last_end = match.end()
  parts.append(md_text[last_end:])
  return "".join(parts), functions


def build_slug_map(base: Path, entries: list[tuple[str, Path]]) -> dict[str, str]:
  mapping: dict[str, str] = {}
  for slug, path in entries:
    try:
      rel = path.relative_to(base)
    except ValueError:
      continue
    rel_posix = rel.as_posix().lstrip("./")
    variants = {rel_posix}
    if rel_posix.endswith("/"):
      variants.add(rel_posix.rstrip("/"))
    if rel.name.lower() == "readme.md":
      parent = rel.parent.as_posix()
      variants.add(parent)
      if parent:
        variants.add(parent.rstrip("/") + "/")
      variants.add(rel_posix[:-3])
    variants.add(rel.with_suffix("").as_posix())
    for key in variants:
      if key not in mapping:
        mapping[key] = slug
  return mapping


def rewrite_links(
    md_text: str, base: Path, page_path: Path, lang: str, slug_map: dict[str, str]
) -> str:
  link_pattern = re.compile(r"\[(?P<label>[^\]]+)\]\((?P<href>[^)]+)\)")
  try:
    current_rel = page_path.relative_to(base).as_posix().lstrip("./")
  except ValueError:
    current_rel = ""

  def replace(match: re.Match[str]) -> str:
    label = match.group("label")
    href = match.group("href")
    if re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*://", href) or href.startswith("mailto:"):
      return match.group(0)
    if href.startswith("#"):
      anchor = href[1:]
      current_slug = slug_map.get(current_rel, "")
      if not current_slug:
        return match.group(0)
      target = f"#/{lang}/{current_slug}"
      if anchor:
        target += f"/{anchor}"
      return f"[{label}]({target})"
    anchor = ""
    path_part = href
    if "#" in href:
      path_part, anchor = href.split("#", 1)
    resolved = (page_path.parent / path_part).resolve()
    try:
      rel = resolved.relative_to(base).as_posix().lstrip("./")
    except ValueError:
      return match.group(0)
    if rel.endswith("/"):
      rel = rel.rstrip("/")
    candidates = [rel]
    if rel.endswith("README.md"):
      parent = Path(rel).parent.as_posix()
      candidates.append(parent)
    candidates.append(Path(rel).with_suffix("").as_posix())
    slug = None
    for candidate in candidates:
      if candidate in slug_map:
        slug = slug_map[candidate]
        break
    if not slug:
      return match.group(0)
    target = f"#/{lang}/{slug}"
    if anchor:
      target += f"/{anchor}"
    return f"[{label}]({target})"

  return link_pattern.sub(replace, md_text)


def add_page(
    pages: list[dict[str, str]],
    slug: str,
    path: Path,
    fallback_label: str,
    base: Path,
    lang: str,
    slug_map: dict[str, str],
) -> None:
  if not path.exists():
    return
  md_text = read_markdown(path)
  functions: list[dict[str, str]] = []
  if slug.startswith("api-"):
    md_text, functions = extract_functions(md_text, slug)
  for function in functions:
    function["title"] = strip_signature_modifiers(function["title"])
  md_text = rewrite_links(md_text, base, path, lang, slug_map)
  html_text = convert_markdown(md_text)
  title = extract_title(md_text, fallback_label)
  pages.append(
      {
          "slug": slug,
          "title": title,
          "label": title,
          "html": html_text,
          "text": to_plain_text(html_text),
          "functions": functions,
      }
  )


def collect_language(root: Path, lang: str) -> list[dict[str, str]]:
  # todo: give labels in the right language
  base = root / lang
  if not base.is_dir():
    raise FileNotFoundError(f"Missing language directory: {base}")
  entries: list[tuple[str, Path, str]] = []
  entries.append(("home", base / "README.md", "Overview"))
  entries.append(
      ("getting-started", base / "getting-started" / "README.md", "Getting started")
  )
  entries.append(("tutorial", base / "tutorial" / "README.md", "Tutorial"))
  entries.append(("api-index", base / "api" / "README.md", "API"))
  api_dir = base / "api"
  if api_dir.is_dir():
    for entry in sorted(api_dir.iterdir()):
      if entry.is_dir():
        entries.append((f"api-{entry.name}", entry / "README.md", entry.name))
  slug_map = build_slug_map(base, [(slug, path) for slug, path, _ in entries])
  pages: list[dict[str, str]] = []
  for slug, path, label in entries:
    add_page(pages, slug, path, label, base, lang, slug_map)
  return pages


def detect_languages(root: Path) -> list[str]:
  languages = []
  for entry in root.iterdir():
    if entry.is_dir() and (entry / "README.md").exists():
      languages.append(entry.name)
  ordered = [lang for lang in DEFAULT_LANGS if lang in languages]
  remaining = [lang for lang in languages if lang not in ordered]
  return ordered + sorted(remaining)


def build_payload(root: Path, languages: list[str]) -> dict[str, list[dict[str, str]]]:
  payload: dict[str, list[dict[str, str]]] = {}
  for lang in languages:
    payload[lang] = collect_language(root, lang)
  return payload


def write_html(output_path: Path, data: dict[str, list[dict[str, str]]], start_lang: str) -> None:
  data_json = json.dumps(data, ensure_ascii=False)
  lang_json = json.dumps(LANG_NAMES, ensure_ascii=False)
  data_json = data_json.replace("</", "<\\/")
  lang_json = lang_json.replace("</", "<\\/")
  html_text = (
      HTML_TEMPLATE.replace("__DOCS__", data_json)
      .replace("__LANG_MAP__", lang_json)
      .replace("__DEFAULT_LANG__", start_lang)
  )
  output_path.parent.mkdir(parents=True, exist_ok=True)
  output_path.write_text(html_text, encoding="utf-8")


def parse_args() -> argparse.Namespace:
  parser = argparse.ArgumentParser(description="Build attoboy docs SPA from Markdown.")
  parser.add_argument(
      "--root",
      default=Path(__file__).parent,
      type=Path,
      help="Docs root directory containing language folders.",
  )
  parser.add_argument(
      "--output",
      default=Path(__file__).parent / "html" / "index.html",
      type=Path,
      help="Destination HTML file.",
  )
  parser.add_argument(
      "--languages",
      nargs="*",
      help="Languages to include (default: detected).",
  )
  return parser.parse_args()


def main() -> None:
  args = parse_args()
  root = args.root.resolve()
  if args.languages:
    languages = args.languages
  else:
    languages = detect_languages(root)
  if not languages:
    raise SystemExit("No languages found.")
  start_lang = languages[0]
  data = build_payload(root, languages)
  write_html(args.output, data, start_lang)


if __name__ == "__main__":
  main()


