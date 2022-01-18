
export function create_card(stub) {
    let card = document.createElement("div");
    let element = document.createElement("div");
    let subject = document.createElement("div");

    card.setAttribute("class", "card_animate");
    element.setAttribute("class", "card");
    subject.setAttribute("class", "subject");

    let sub1 = document.createElement("div");
    let sub2 = document.createElement("div");
    subject.appendChild(sub1);
    subject.appendChild(sub2);
    element.appendChild(subject);
    sub1.innerText = stub.subject;
    if (stub.subject.length > 20) {
        sub1.innerText = stub.subject.slice(0, 20) + "...";
    } else {
        sub1.innerText = stub.subject;
    }
    if (stub.from.length > 20) {
        sub2.innerText = stub.from.slice(0, 20) + "...";
    } else {
        sub2.innerText = stub.from;
    }
    card.appendChild(element);

    let options = document.createElement("div")
    options.setAttribute("class", "options")
    let delete_b = document.createElement("div")
    delete_b.setAttribute("class", "ui_delete")
    delete_b.innerText = "Delete";
    options.appendChild(delete_b)
    element.appendChild(options)

    return card;
}

export function create_user_card(username, role) {
    let card = document.createElement("div");
    let element = document.createElement("div");
    let subject = document.createElement("div");

    element.setAttribute("class", "card");
    subject.setAttribute("class", "subject");

    let sub1 = document.createElement("div");
    let sub2 = document.createElement("div");
    subject.appendChild(sub1);
    subject.appendChild(sub2);
    element.appendChild(subject);
    sub1.innerText = username;
    sub2.innerText = role;
    card.appendChild(element);

    let options = document.createElement("div")
    options.setAttribute("class", "options")
    let delete_b = document.createElement("div")
    delete_b.setAttribute("class", "ui_delete")
    delete_b.innerText = "Delete";
    options.appendChild(delete_b)
    element.appendChild(options)

    return card;
}