// scrollについての処理記述

window.addEventListener('DOMContentLoaded', function () {
    registClickLinkScroll();
    switchWatchingContents();
    for (const toTop of document.querySelectorAll('a.scroll-top')) {
        toTop.addEventListener('click', function (e) {
            e.preventDefault();
            scrollTop();
        });
    }
});

// 画面読み込み直後のずれ補正
window.addEventListener('load', function () {
    if (location.hash.length) {
        scrollToContentsId(location.hash);
    }
});

window.addEventListener('scroll', switchWatchingContents);

/**
 * リンク押下時のスクロールイベント登録
 */
function registClickLinkScroll() {
    const anchors = document.querySelectorAll('a[href*="#"]');
    for (const a of anchors) {
        // 画面内リンクの場合のみ登録
        if (a.pathname.length && a.pathname !== location.pathname || !a.hash.length) {
            continue;
        }
        
        a.addEventListener('click', function (e) {
            e.preventDefault();
            scrollToContentsId(a.hash);
        });
    }
}

/**
 * 画面の一番上までスクロールする
 */
function scrollTop() {
    window.scrollTo({
      top: 0,
      behavior: "smooth"
    });
}

/**
 * 指定IDの位置までスクロールする
 * @param {string} id 
 */
function scrollToContentsId(id) {
    const headerHeight = document.getElementById('header').offsetHeight;
    const targetElement = document.querySelector(decodeURI(id));
    if (!targetElement) {
        return;
    }

    const targetOffsetTop = window.scrollY + targetElement.getBoundingClientRect().top - headerHeight;
    window.scrollTo({
      top: targetOffsetTop,
      behavior: "smooth"
    });
}

/**
 * 現在のスクロール位置より、該当する目次にハイライトを付ける
 */
function switchWatchingContents() {
    const headerHeight = document.getElementById('header').offsetHeight;
    const tableOfContents = document.querySelectorAll('#table_of_contents li > a');

    // watchingクラスを除去
    const watching = document.querySelector('#table_of_contents li.watching');
    if (watching) {
        watching.classList.remove('watching');
    }

    const itemRelativePositions = [];
    for (const item of tableOfContents) {
        const targetElement = document.querySelector(decodeURI(item.hash));
        if (!targetElement) {
            continue;
        }

        // 参照中の要素と、表示中画面からの相対位置を収集する
        itemRelativePositions.push({
            item: item,
            // 一文字くらい余裕を持たせるため-10(目次リンクをクリックした場合、色が変わらない場合があるため)
            positionY: -(targetElement.getBoundingClientRect().top - headerHeight - 10),
        });
    }
    
    // マイナスを除去し、昇順で並び替えた先頭を参照中とする
    const sorted = itemRelativePositions.filter(pos => pos.positionY >= 0)
        .sort((pos1, pos2) => pos1.positionY - pos2.positionY);
    if (sorted.length) {
        sorted[0].item.parentElement.classList.add('watching');
    }
}
