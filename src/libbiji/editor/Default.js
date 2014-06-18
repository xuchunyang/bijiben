function myFunction() {
    document.getElementById("demo").innerHTML = Date();
}

// Return: Selection html as string
function getSelectionHtml() {
    var html = "";
    if (typeof window.getSelection != "undefined") {
        var sel = window.getSelection();
        if (sel.rangeCount) {
            var container = document.createElement("div");
            for (var i = 0, len = sel.rangeCount; i < len; ++i) {
                container.appendChild(sel.getRangeAt(i).cloneContents());
            }
            html = container.innerHTML;
        }
    } else if (typeof document.selection != "undefined") {
        if (document.selection.type == "Text") {
            html = document.selection.createRange().htmlText;
        }
    }
    return html;
}

// change background
function setBackgroundColor(r, g, b, a) {
    color = "rgba(" + r + "," + g + "," + b + "," + a + ")";
    document.body.style.backgroundColor=color;
}

function getNote()
{
    var note = document.getElementById("note");
    var x = document.getElementById("demo");
    x.innerHTML = note.textContent;
}
