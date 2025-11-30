# Builds a single-page HTML app for the attoboy documentation in all languages.
# Usage: python docs2html.py --output html/index.html
# Requires the Python `markdown` package listed in requirements.txt.

import argparse
import base64
import html
import json
import re
import zlib
from pathlib import Path

import markdown

LANG_NAMES = {"en": "EN", "ru": "РУ", "zh": "中"}
DEFAULT_LANGS = ("en", "ru", "zh")

def slugify(value: str) -> str:
  value = value.lower()
  value = re.sub(r"[^\w]+", "-", value)
  return value.strip("-") or "item"


def compress_text(text: str) -> str:
  """Compress text with DEFLATE and return as base64 string."""
  compressor = zlib.compressobj(level=9, wbits=-15)
  compressed = compressor.compress(text.encode("utf-8")) + compressor.flush()
  return base64.b64encode(compressed).decode("ascii")


HTML_TEMPLATE = r"""
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>ttoboy Documentation</title>
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
.brand-icon { width: 44px; height: 44px; border-radius: 12px; overflow: hidden; display: block; text-decoration: none; }
.brand-icon img { width: 100%; height: 100%; object-fit: cover; }
.brand-text { display: flex; flex-direction: column; gap: 2px; }
.brand-title { font-size: 18px; font-weight: 700; }
.brand-sub { font-size: 12px; color: var(--muted); }
.top-actions { display: flex; align-items: center; gap: 12px; flex: 1; justify-content: flex-end; }
.github-link { padding: 6px 8px 2px 8px; border-radius: 10px; border: 1px solid var(--border); color: #eef2ff; background: #0f164b; box-shadow: var(--shadow); transition: transform 0.15s ease, border-color 0.15s ease; }
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
.tok-number { color: #ff9080 }
.tok-comment { color: #8291b4; }
.tok-directive { color: #f0a8ff; }
.tok-attoboy { color: #ffff80; }
@media (max-width: 980px) {
  .layout { grid-template-columns: 1fr; }
  .sidebar { position: static; height: auto; }
}
</style>
</head>
<body>
<div class="topbar">
  <div class="brand">
    <a class="brand-icon" id="brand-link" href="#"><img src="data:image/png;base64,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" alt="Attoboy logo"></a>
    <div class="brand-text">
      <div class="brand-title">Attoboy</div>
      <div class="brand-sub">C++ library documentation</div>
    </div>
  </div>
  <div class="top-actions">
    <a class="github-link" href="https://github.com/coeurnix/attoboy" target="_blank" rel="noopener" aria-label="View source on GitHub">
      <svg width="1.2em" height="1.2em" viewBox="0 0 1024 1024" fill="currentColor" aria-hidden="true">
        <path fill-rule="evenodd" clip-rule="evenodd" d="M8 0C3.58 0 0 3.58 0 8C0 11.54 2.29 14.53 5.47 15.59C5.87 15.66 6.02 15.42 6.02 15.21C6.02 15.02 6.01 14.39 6.01 13.72C4 14.09 3.48 13.23 3.32 12.78C3.23 12.55 2.84 11.84 2.5 11.65C2.22 11.5 1.82 11.13 2.49 11.12C3.12 11.11 3.57 11.7 3.72 11.94C4.44 13.15 5.59 12.81 6.05 12.6C6.12 12.08 6.33 11.73 6.56 11.53C4.78 11.33 2.92 10.64 2.92 7.58C2.92 6.71 3.23 5.99 3.74 5.43C3.66 5.23 3.38 4.41 3.82 3.31C3.82 3.31 4.49 3.1 6.02 4.13C6.66 3.95 7.34 3.86 8.02 3.86C8.7 3.86 9.38 3.95 10.02 4.13C11.55 3.09 12.22 3.31 12.22 3.31C12.66 4.41 12.38 5.23 12.3 5.43C12.81 5.99 13.12 6.7 13.12 7.58C13.12 10.65 11.25 11.33 9.47 11.53C9.76 11.78 10.01 12.26 10.01 13.01C10.01 14.08 10 14.94 10 15.21C10 15.42 10.15 15.67 10.55 15.59C13.71 14.53 16 11.53 16 8C16 3.58 12.42 0 8 0Z" transform="scale(64)" />
      </svg>
    </a>
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
const FUNCTION_SEARCH_INDEX = __FUNCTION_INDEX__;
let currentLang = "__DEFAULT_LANG__";
let currentSlug = "home";
let currentAnchor = null;
let suppressHashChange = false;

// Cache for decompressed HTML content
const decompressedCache = {};

// Decompress DEFLATE-compressed base64 data
async function decompressContent(base64Data) {
  const binary = atob(base64Data);
  const bytes = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; i++) {
    bytes[i] = binary.charCodeAt(i);
  }
  const stream = new ReadableStream({
    start(controller) {
      controller.enqueue(bytes);
      controller.close();
    }
  });
  const decompressed = await new Response(
    stream.pipeThrough(new DecompressionStream('deflate-raw'))
  ).text();
  return decompressed;
}

// Get decompressed HTML for a page, using cache
async function getPageHtml(lang, slug) {
  const cacheKey = `${lang}:${slug}`;
  if (decompressedCache[cacheKey]) {
    return decompressedCache[cacheKey];
  }
  const page = getPage(lang, slug);
  if (!page) return null;
  const html = await decompressContent(page.c);
  decompressedCache[cacheKey] = html;
  return html;
}

const navList = document.getElementById("nav-list");
const contentEl = document.getElementById("doc-content");
const searchInput = document.getElementById("search-input");
const searchResults = document.getElementById("search-results");
const langSwitch = document.getElementById("language-switch");
const brandLink = document.getElementById("brand-link");

function getPage(lang, slug) {
  const pages = DOCS[lang] || [];
  for (const page of pages) {
    if (page.s === slug) return page;
  }
  return null;
}

function ensureSlug(slug, lang) {
  if (getPage(lang, slug)) return slug;
  if (getPage(lang, "home")) return "home";
  const pages = DOCS[lang] || [];
  return pages.length ? pages[0].s : slug;
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
  const apiPages = pages.filter((p) => p.s.startsWith("api-") && p.s !== "api-index");
  const markup = [];
  for (const slug of baseOrder) {
    const page = getPage(currentLang, slug);
    if (page) {
      const active = slug === currentSlug ? "active" : "";
      markup.push(`<button class="nav-item ${active}" data-slug="${slug}">${page.l}</button>`);
    }
  }
  if (apiPages.length) {
    markup.push('<div class="nav-group-label">API Classes</div>');
    for (const page of apiPages) {
      const active = page.s === currentSlug ? "active" : "";
      markup.push(`<button class="nav-item ${active}" data-slug="${page.s}">${page.l}</button>`);
      if (page.s === currentSlug && page.f && page.f.length) {
        const sub = page.f
          .map((fn) => {
            const activeFn = currentAnchor === fn.i ? "active" : "";
            return `<button class="nav-sub-item ${activeFn}" data-slug="${page.s}" data-anchor="${fn.i}">${fn.t}</button>`;
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
    { type: "string", regex: /"(?:\\.|[^\\"])*"|' (?:\\.|[^\\'])*'/g },
    { type: "number", regex: /\b0[xX][0-9a-fA-F]+(?:[uU][lL]{0,2}|[lL]{1,2}[uU]?)?\b|\b0[bB][01]+(?:[uU][lL]{0,2}|[lL]{1,2}[uU]?)?\b|\b(?:\d[\d_]*\.\d[\d_]*)|\b\d[\d_]*(?:\.\d[\d_]*)?(?:[eE][+-]?\d[\d_]*)?[fFlL]?\b|\b\.\d[\d_]*(?:[eE][+-]?\d[\d_]*)?[fFlL]?\b/g },
    { type: "keyword", regex: /\b(?:alignas|alignof|asm|auto|bool|break|case|catch|char|class|const|constexpr|continue|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|nullptr|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while)\b/g },
    { type: "directive", regex: /#[ \t]*[a-zA-Z_]\w*/g },
    { type: "attoboy", regex: /\b(?:String|List|Map|Set|Arguments|AI|Buffer|Console|Conversation|DateTime|Embedding|File|Math|Mutex|Path|Registry|Subprocess|Thread|Log|LogInfo|LogError|LogDebug|LogWarning|WebRequest|WebResponse|atto_main|attoboy::|Exit)\b/g },    
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

async function renderPage() {
  const page = getPage(currentLang, currentSlug);
  if (!page) return;
  
  // Get decompressed HTML content
  const htmlContent = await getPageHtml(currentLang, currentSlug);
  contentEl.innerHTML = htmlContent;
  document.title = page.t + " | attoboy";
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
  return value.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

function highlightTerm(text, term) {
  if (!term) return escapeHtml(text);
  const safe = escapeHtml(text);
  const re = new RegExp("(" + escapeRegex(term) + ")", "ig");
  return safe.replace(re, "<mark>$1</mark>");
}

function runSearch(term) {
  const query = term.trim();
  if (!query) {
    closeSearch();
    return;
  }
  const results = [];
  const qLower = query.toLowerCase();
  for (const fn of FUNCTION_SEARCH_INDEX) {
    if (fn.s.toLowerCase().includes(qLower)) {
      results.push(fn);
    }
    if (results.length === 8) break;
  }
  if (!results.length) {
    searchResults.innerHTML = '<div class="result"><div class="snippet">No matches found</div></div>';
    searchResults.classList.add("open");
    return;
  }
  searchResults.innerHTML = results
    .map((fn) => {
      const sig = highlightTerm(fn.s, query);
      return `<div class="result" data-slug="${fn.p}" data-anchor="${fn.i}">
        <div class="title">${sig}</div>
        <div class="snippet"><span class="muted">${fn.l}</span></div>
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
    body = re.sub(r'\*\*Signature\*\*.*?(?=\*\*Synopsis\*\*)', '', body, flags=re.DOTALL)
    header = re.sub(r'####', '###', header)
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
  
  html_compressed = compress_text(html_text)
  
  # Use short keys: s=slug, t=title, l=label, c=compressed content, f=functions (ids only for nav)
  page_data = {
      "s": slug,
      "t": title,
      "l": title,
      "c": html_compressed,
  }
  
  # Store minimal function info for navigation (just id and title for sidebar)
  if functions:
    page_data["f"] = [{"i": f["id"], "t": f["title"]} for f in functions]
  
  pages.append((page_data, functions, title))


def collect_language(root: Path, lang: str) -> tuple[list[dict], list[dict]]:
  """Returns (pages, function_index) where function_index is only populated for English."""
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
  page_results: list[tuple] = []
  for slug, path, label in entries:
    add_page(page_results, slug, path, label, base, lang, slug_map)
  
  # Extract pages and build function index (for English only)
  pages = []
  function_index = []
  for page_data, functions, title in page_results:
    pages.append(page_data)
    # Build function search index from English only
    # Only include signatures that look like C++ (contain :: or parentheses)
    if lang == "en" and functions:
      for fn in functions:
        sig = fn["signature"]
        if "::" in sig or "(" in sig:
          function_index.append({
              "s": sig,          # signature for search
              "i": fn["id"],     # anchor id
              "p": page_data["s"],  # page slug
              "l": title,        # page label
          })
  
  return pages, function_index


def detect_languages(root: Path) -> list[str]:
  languages = []
  for entry in root.iterdir():
    if entry.is_dir() and (entry / "README.md").exists():
      languages.append(entry.name)
  ordered = [lang for lang in DEFAULT_LANGS if lang in languages]
  remaining = [lang for lang in languages if lang not in ordered]
  return ordered + sorted(remaining)


def build_payload(root: Path, languages: list[str]) -> tuple[dict[str, list[dict]], list[dict]]:
  """Returns (docs_by_language, function_search_index)."""
  payload: dict[str, list[dict]] = {}
  function_index: list[dict] = []
  for lang in languages:
    pages, lang_functions = collect_language(root, lang)
    payload[lang] = pages
    if lang == "en":
      function_index = lang_functions
  return payload, function_index


def write_html(output_path: Path, data: dict[str, list[dict]], function_index: list[dict], start_lang: str) -> None:
  data_json = json.dumps(data, ensure_ascii=False, separators=(',', ':'))
  lang_json = json.dumps(LANG_NAMES, ensure_ascii=False, separators=(',', ':'))
  func_json = json.dumps(function_index, ensure_ascii=False, separators=(',', ':'))
  data_json = data_json.replace("</", "<\\/")
  lang_json = lang_json.replace("</", "<\\/")
  func_json = func_json.replace("</", "<\\/")
  html_text = (
      HTML_TEMPLATE.replace("__DOCS__", data_json)
      .replace("__LANG_MAP__", lang_json)
      .replace("__FUNCTION_INDEX__", func_json)
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
  data, function_index = build_payload(root, languages)
  write_html(args.output, data, function_index, start_lang)


if __name__ == "__main__":
  main()