// 開閉制御

import { existsLocationPathLink } from "./pagepath.js";

window.addEventListener('DOMContentLoaded', function() {
    // サイドバーの必要な個所にtoggleボタンを付ける
    const nestedSideberList = window.document.querySelectorAll('#sidebar > ul ul');
    for (const ul of nestedSideberList) {
        const toggleBtn = makeToggleBtn(ul, existsLocationPathLink(ul.parentElement));
        ul.parentElement.append(toggleBtn);
    }
});

/**
 * 開閉ボタン生成
 * 開閉対象の要素を渡すことで、ボタン押下時の動作を開閉対象ボタンへ適用する
 * @param {HTMLElement} targetElement
 * @param {boolean} isDefaultActive デフォルトの開閉状態
 */
function makeToggleBtn(targetElement, isDefaultActive = false) {
    const button = window.document.createElement('button');
    const defaultDisplay = targetElement.style.display;
    button.classList.add('toggle-btn');

    // 押下時のイベント登録(対象要素を開閉する)
    button.addEventListener('click', function() {
        button.classList.toggle('active');
        targetElement.style.display = button.classList.contains('active')
            ? defaultDisplay
            : 'none';
    });

    // 逆の開閉状態を指定したうえでクリックし、初期状態とする
    button.classList.toggle('active', !isDefaultActive);
    button.click();
    return button;
}
