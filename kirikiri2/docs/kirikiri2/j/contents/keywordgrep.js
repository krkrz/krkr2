var spans = document.getElementsByTagName('span');
function grep(str) {
  var regs = [];
  var words = str.split(' ');
  for (var i = 0; i < words.length; ++i) {
    if (words[i] != '') {
      regs.push(new RegExp(words[i].replace(/(\W)/g, "\\$1"), 'i'));
    }
  }

  for (var i = 0; i < spans.length; ++i) {
    var span = spans[i];
    var text1 = span.parentNode.childNodes[0].nodeValue;
    var text2 = span.childNodes[0].nodeValue;

    var matched = true;
    for (var j = 0; j < regs.length; ++j) {
      if (!regs[j].test(text1) && !regs[j].test(text2)) {
        matched = false;
        break;
      }
    }

    if (matched) {
      span.parentNode.parentNode.parentNode.parentNode.style.display = '';
    } else {
      span.parentNode.parentNode.parentNode.parentNode.style.display = 'none';
    }
  }
}
