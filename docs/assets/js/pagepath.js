// 画面やaタグ等のパス情報制御

/**
 * 指定 selector 以下に存在する本画面と同じ pathname のanchorタグが存在するか判定
 * @param {string|HTMLElement} selector 空文字の場合、画面全体を対象に検索実施
 */
export function existsLocationPathLink(selector) {
    return Boolean(getLocationPathLink(selector));
}

/**
 * 指定 selector 以下に存在する本画面と同じ pathname の最初に見つかったanchorタグを取得する
 * @param {string|HTMLElement} selector 空文字の場合、画面全体を対象に検索実施
 * @return {HTMLAnchorElement|null} 見つからなかったらnull
 */
export function getLocationPathLink(selector = "") {
    const anchors = getLocationPathLinkAll(selector);
    return anchors.length ? anchors[0] : null;
}

/**
 * 指定 selector 以下に存在する本画面と同じ pathname のanchorタグを全て取得する
 * @param {string|HTMLElement} selector 空文字の場合、画面全体を対象に検索実施
 * @return {HTMLAnchorElement[]}
 */
export function getLocationPathLinkAll(selector = "") {
    return [...getSelectorUnderAnchors(selector)]
        .filter(a => isLocationPathname(a));
}

/**
 * 指定パスが本画面のものと一致しているか
 * @param {*} path 
 */
export function isLocationPathname(path) {
    return eqPathname(window.location, path);
}

/**
 * 文字列または、pathnameプロパティを持つオブジェクトが同じpathnameを持つか比較  
 * index.htmlの有無も吸収
 * @param {*} path1 
 * @param {*} path2 
 */
export function eqPathname(path1, path2) {
    const compVal1 = getPathnameCompareValue(path1).replace('index.html', '');
    const compVal2 = getPathnameCompareValue(path2).replace('index.html', '');
    return compVal1 === compVal2;
}

/**
 * 指定オブジェクトからpathnameと比較するための値を取り出す  
 * 文字列ではない場合も強制的に文字列型に変換される
 * @param {*} path 
 */
function getPathnameCompareValue(path) {
    return (!path || !path.pathname)
        ? String(path)
        : String(path.pathname);
}

/**
 * 文字列とHTMLの型の違いを吸収したうえで、配下の anchor要素を全て取得する
 * @param {string|HTMLElement} selector 
 * @returns 
 */
function getSelectorUnderAnchors(selector) {
    if (typeof selector === "string" || selector instanceof String) {
        return window.document.querySelectorAll(selector + " a");
    }

    return selector instanceof HTMLElement
        ? selector.querySelectorAll('a')
        : [];
}
