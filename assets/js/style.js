// スタイル設定補助用スクリプト
import { getLocationPathLinkAll } from "./pagepath.js";

(function() {
    // prism用(サンプルの行表示)
    for (const pre of window.document.getElementsByTagName('pre')) {
        pre.classList.add('line-numbers');
    }
})();

window.addEventListener('DOMContentLoaded', function(e) {
    resize();

    // メニューのハイライト
    for (const item of getLocationPathLinkAll('#sidebar li >')) {
        item.parentElement.classList.add('watching');
    }
});

window.addEventListener('resize', resize);

/**
 * 画面リサイズした際の処理
 **/
function resize() {
    resizeSidebar(window.document.getElementById('sidebar'));
    resizeSidebar(window.document.getElementById('table_of_contents'));
}

/**
 * サイドバーの高さと鉛直方向位置調整
 * @param {HTMLElement} sidebarElem
 **/
function resizeSidebar(sidebar) {
    const header = window.document.getElementById('header');
    const footer = window.document.getElementById('footer');
    const sidebarTopPos = header.offsetHeight;
    const maxHeight = window.innerHeight - sidebarTopPos;

    sidebar.style.top = sidebarTopPos + 'px';
    sidebar.style.maxHeight = maxHeight + 'px';
    // よくわからないけど、2pxウィンドウより大きくなってしまう
    sidebar.style.minHeight = (maxHeight - footer.offsetHeight - 2) + 'px';
}
